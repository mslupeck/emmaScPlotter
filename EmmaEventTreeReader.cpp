/*
 * emmaEventTreeReader.cpp
 *
 *  Created on: Nov 18, 2019
 *      Author: mss
 */

#include "EmmaEventTreeReader.h"
#include "TEventAnalysis.h"

namespace std {

EmmaEventTreeReader::EmmaEventTreeReader() {
	vCol.push_back(kRed);
	vCol.push_back(kMagenta);
	vCol.push_back(kBlue);
	vCol.push_back(kBlack);
	vCol.push_back(kGray+1);
	for(int icol=5; icol<8; icol++){
		vCol.push_back(kGreen-2);
	}
}

EmmaEventTreeReader::~EmmaEventTreeReader(){
}

void EmmaEventTreeReader::SetInputFileInfo(const string &filePath, const string &treeName, const string &objectName){
	this->filePath = filePath;
	this->treeName = treeName;
	this->objectName = objectName;
	fileBaseName = filePath.substr(filePath.find_last_of('/')+1);
	fileBaseName = fileBaseName.substr(0, fileBaseName.find_last_of('.'));
}

int EmmaEventTreeReader::ReadTreeFromRootFile(){
	cout << "  <I> Opening file: " << filePath << endl;
	TFile *f = new TFile(filePath.c_str(), "READ"); // try opening the root file
	if(f == nullptr){
		cout << "<E> Problems opening file: " << filePath << endl;
		return -1; // exit program
	}

	TTree *t = (TTree*)(f->Get(treeName.c_str())); // get the tree from file
	if(t == nullptr){
		cout << "<E> Problems opening tree: " << treeName << endl;
		return -1; // exit program
	}

	TFileStorage *fs = nullptr;
	t->SetBranchAddress(objectName.c_str(), &fs);
	if(fs == nullptr){
		cout << "<E> Problems opening event object: " << objectName << endl;
		return -1; // exit program
	}

	// Copy tree to vector in memory
	int nEntries = t->GetEntries();
	for(int evn=0; evn<nEntries; evn++){
		t->GetEntry(evn);
		vfs.push_back(new TFileStorage(*fs));
	}

	f->Close();
	return 0;
}

void EmmaEventTreeReader::PrintStorageContents(ostream &out, int64_t evn){
	int nEntries = vfs.size();
	if(evn<nEntries){
		TFileStorage *fs = vfs.at(evn);
		out << left;
		out << "R" << fs->iRunNumber << "-F" << setw(3) << fs->iFileNumber+100 << ":" << evn << endl;
		out << "  StartTime:      " << right << setw(12) << fs->lFileStartTimeS << "." << left << setw(10) << fs->lFileStartTimeNs << "s  /  ";
		out << ctime((time_t*)&(fs->lFileStartTimeS));
		out << "  FileDuration [s]: " << fs->fFileDuration << " +/- " << fs->fFileDurationUncertainty << endl;
		out << "  FileT0 [s]:       " << fs->fFileT0 << endl;
		out << "  EventTime [s]:    " << fs->fEventTimeS << endl;
		out << "    Flags: ";
		for(UInt_t iflag=0; iflag<fs->vFlag.size(); iflag++){
			TFlagStorage *flag = &(fs->vFlag.at(iflag));
			for(UInt_t idata=0; idata<flag->ch.size(); idata++){
				out << flag->ch.at(idata) << "(" << flag->t.at(idata) << ")  ";
			}
		}
		out << endl;
		out << "    Hits: " << endl;
		for(UInt_t ihit=0; ihit<fs->vHitPoint.size(); ihit++){
			THitStorage *hs = &(fs->vHitPoint.at(ihit));
			out << "      SC" << right << setfill('0') << setw(2) << hs->scModule << "-";
			out << setw(2) << hs->scPixel << setfill(' ') << " (";
			out << setw(4) << hs->x << ", " << setw(4) << hs->y << ", " << setw(4) << hs->z << "); " << setw(6) << hs->t << endl;
		}
	}
}

void EmmaEventTreeReader::AnalysePatternTimingCorrelation(){
	cout << "  <I> EmmaEventTreeReader::AnalysePatternTimingCorrelation()" << endl;
	// Initialize storage vectors
	vector<int> vTimeCnt;
	vector<int> vHbCnt;
	vector<int> vTimeAndHbCnt;
	vector<int> vTimeOrHbCnt;
	vector<bool> vTimePresent;
	vector<bool> vHbPresent;
	const int nSc = 99;
	for(int isc=0; isc<nSc; isc++){
		vTimePresent.push_back(false);
		vHbPresent.push_back(false);
		vTimeCnt.push_back(0);
		vHbCnt.push_back(0);
		vTimeAndHbCnt.push_back(0);
		vTimeOrHbCnt.push_back(0);
	}

	// Main event loop
	int nEntries = vfs.size();
	for(int evn=0; evn<nEntries; evn++){
		TEventAnalysis ea(&(vfs.at(evn)->vHitPoint), &cuts);
		ea.FillHbTbHistos(vTimeCnt, vHbCnt, vTimeAndHbCnt, vTimeOrHbCnt);
	}

	// Print results
	cout << " SC      TimeOrHb        Time          Hb   TimeAndHb" << endl;
	for(int isc=0; isc<nSc; isc++){
		if((vTimeCnt.at(isc) > 0) || (vHbCnt.at(isc) > 0)){
			cout << "SC" << right << setfill('0') << setw(2) << isc << setfill(' ') << "  ";
			cout << setw(11) << vTimeOrHbCnt.at(isc);
			cout << setw(11) << round((float)vTimeCnt.at(isc)/vTimeOrHbCnt.at(isc)*1000)/10 << "%";
			cout << setw(11) << round((float)vHbCnt.at(isc)/vTimeOrHbCnt.at(isc)*1000)/10 << "%";
			cout << setw(11) << round((float)vTimeAndHbCnt.at(isc)/vTimeOrHbCnt.at(isc)*1000)/10 << "%" << endl;
		}
	}

	// Plot results
	string sCvsName = "cTB_HB_" + fileBaseName;
	TCanvas *c = new TCanvas(sCvsName.c_str(), sCvsName.c_str(), 1600, 600);
	c->cd();

	int nActiveSc16s = 0;
	for(int isc=0; isc<nSc; isc++){
		if((vTimeCnt.at(isc) > 0) || (vHbCnt.at(isc) > 0)){
			nActiveSc16s++;
		}
	}

	vector<string> vhName;
	vhName.push_back("TB");
	vhName.push_back("HB");
	vhName.push_back("TB-and-HB");
	vector<Color_t> vCol;
	vCol.push_back(kBlack);
	vCol.push_back(kRed);
	vCol.push_back(kBlue);
	vector<TH1D*> vh;
	for(UInt_t ih=0; ih<vhName.size(); ih++){
		vh.push_back(new TH1D(vhName.at(ih).c_str(), "", nActiveSc16s, 0, nActiveSc16s));
		vh.at(ih)->SetDirectory(0);
		vh.at(ih)->SetLineWidth(2);
		vh.at(ih)->SetLineColor(vCol.at(ih));
	}

	int sc16cnt = 0;
	for(int isc=0; isc<nSc; isc++){
		if((vTimeCnt.at(isc) > 0) || (vHbCnt.at(isc) > 0)){
			stringstream ssBinLabelName;
			ssBinLabelName << isc;
			vh.at(0)->SetBinContent(sc16cnt+1, (float)vTimeCnt.at(isc)/vTimeOrHbCnt.at(isc));
			vh.at(1)->SetBinContent(sc16cnt+1, (float)vHbCnt.at(isc)/vTimeOrHbCnt.at(isc));
			vh.at(2)->SetBinContent(sc16cnt+1, (float)vTimeAndHbCnt.at(isc)/vTimeOrHbCnt.at(isc));
			vh.at(0)->GetXaxis()->SetBinLabel(sc16cnt+1, ssBinLabelName.str().c_str());
			vh.at(1)->GetXaxis()->SetBinLabel(sc16cnt+1, ssBinLabelName.str().c_str());
			vh.at(2)->GetXaxis()->SetBinLabel(sc16cnt+1, ssBinLabelName.str().c_str());
			sc16cnt++;
		}
	}

	gStyle->SetOptStat(0);
	float fontsize = 0.06;
	for(UInt_t ih=0; ih<vh.size(); ih++){
		TH1D* h = vh.at(ih);
		if(ih==0){
			h->Draw();
			h->GetYaxis()->SetRangeUser(0,1.1);
			h->GetYaxis()->SetTitle("Ratio by TB-or-HB");
			h->GetXaxis()->SetTitle("SC16 number");
			common::SetupPad(h, fontsize, 0.07, 0.01, 0.01, 0.14, 1.0, 0.55);
		}
		else{
			h->Draw("same");
		}
		common::DrawTextNdc(vhName.at(ih), 0.15, 0.4-fontsize*ih, fontsize, h->GetLineColor());
	}
	stringstream ssnActiveSc16s;
	ssnActiveSc16s << "nActiveSc16s = " << nActiveSc16s << endl;
	common::DrawTextNdc(ssnActiveSc16s.str().c_str(), 0.03, 0.02, fontsize, kBlack);
}

void EmmaEventTreeReader::AnalyseEventTimeDiffSpectrum(){
	cout << "  <I> EmmaEventTreeReader::AnalyseEventTimeDiffSpectrum" << endl;
	string hName = "eventTimeDiff";
	string hName2 = "eventTimeDiffVsFile";
	TH1D* hEventTime = new TH1D(hName.c_str(), hName.c_str(), 1e6, 0, 8000);
	TH2D* hEventTimeVsFile = new TH2D(hName2.c_str(), hName2.c_str(), 1e3/4, 0, 8/4, 1e3, 0, 1e3);
	hEventTime->SetDirectory(0);
	hEventTimeVsFile->SetDirectory(0);

	// Main event loop
	int nEntries = vfs.size();
	Double_t prevEventTime = 0;
	Int_t prevFileNumber = 0;
	for(int evn=0; evn<nEntries; evn++){
		TFileStorage *fs = vfs.at(evn);
		TEventAnalysis ea(&(fs->vHitPoint), &cuts);
		if(evn>0){
			if(prevFileNumber == fs->iFileNumber){ // don't check upon the change of run because the event time diff will be negative
				if(fs->fEventTimeS-prevEventTime < 0){
					cout << " <W> EmmaEventTreeReader::AnalyseEventTimeDiffSpectrum():" << endl;
					cout << "       Wth: " << fs->iFileNumber << "   " << fs->fEventTimeS << "  " << prevEventTime << endl;
				}
				else if((fs->fEventTimeS-prevEventTime)*1e6 < 80){
					cout << " <W> EmmaEventTreeReader::AnalyseEventTimeDiffSpectrum():" << endl;
					cout << "       Events very closeby in time: " << (fs->fEventTimeS-prevEventTime)*1e6 << " us for file: " << fs->iFileNumber+100 << endl;
					cout << "       Discard the offending file before analysing further" << endl;
				}
				hEventTime->Fill((fs->fEventTimeS-prevEventTime)*1e3);
				hEventTimeVsFile->Fill((fs->fEventTimeS-prevEventTime)*1e3, fs->iFileNumber+100);
			}
		}
		prevEventTime = fs->fEventTimeS;
		prevFileNumber = fs->iFileNumber;
	}

	// Plot histo
	float lmargin = 0.115, rmargin = 0.02, tmargin = 0.01, bmargin = 0.1;
	float fontsize = 0.045;
	float xoffset = 1.1, yoffset = 1.25;
	gStyle->SetOptStat(0);
	TCanvas *c = new TCanvas((hName+"_"+fileBaseName).c_str(), (hName+"_"+fileBaseName).c_str(), 1600,800);
	c->Divide(2);
	hEventTime->SetLineWidth(2);
	hEventTime->SetLineColor(kRed-2);
	hEventTime->GetXaxis()->SetTitle("eventTime(n)-eventTime(n-1) [ms]");
	hEventTime->GetYaxis()->SetTitle("N, number of events per 8 #mus");
	hEventTime->SetTitle("");
	float ymax = hEventTime->GetMaximum();
	float nTotal = hEventTime->Integral();
	c->cd(2);
		common::SetupPad(hEventTime, fontsize, lmargin, rmargin, tmargin, bmargin, xoffset, yoffset);
		gPad->SetLogy();
		hEventTime->GetXaxis()->SetRangeUser(0, 2);
		hEventTime->GetYaxis()->SetRangeUser(1e-2, ymax*2);
		hEventTime->DrawCopy();

		float int0=0.0, int1=0.2, int2=1.6, int3=1.8;
		stringstream ss1, ss2;
		float nPeak1 = hEventTime->Integral(hEventTime->GetXaxis()->FindBin(int0), hEventTime->GetXaxis()->FindBin(int1));
		float nPeak2 = hEventTime->Integral(hEventTime->GetXaxis()->FindBin(int2), hEventTime->GetXaxis()->FindBin(int3));
		ss1 << "#frac{N_{peak1}(" << int0 << ".." << int1 << ")}{N_{total}} = " << round(nPeak1/nTotal*1000)/10 << "%";
		ss2 << "#frac{N_{peak2}(" << int2 << ".." << int3 << ")}{N_{total}} = " << round(nPeak2/nTotal*1000)/10 << "%";
		common::DrawTextNdc(ss1.str().c_str(), 0.24, 0.8, fontsize);
		common::DrawTextNdc(ss2.str().c_str(), 0.6, 0.35, fontsize);

	c->cd(1);
		common::SetupPad(hEventTime, fontsize, lmargin, rmargin, tmargin, bmargin, xoffset, yoffset);
		gPad->SetLogy();
		float scale = 100;
		hEventTime->Rebin(scale);
		hEventTime->Scale(1./scale);
		hEventTime->GetXaxis()->SetRangeUser(0, hEventTime->GetXaxis()->GetXmax()-500);
		hEventTime->GetYaxis()->SetRangeUser(1e-2, ymax*2);
		hEventTime->DrawCopy();
		stringstream ss3;
		ss3 << "nTotal = " << nTotal;
		common::DrawTextNdc(ss3.str().c_str(), 0.5, 0.92, fontsize);

	TCanvas *cVsFiles = new TCanvas((hName2+"_"+fileBaseName).c_str(), (hName+"_"+fileBaseName).c_str(), 1600,800);
	cVsFiles->cd();
	gPad->SetLogz();
	hEventTimeVsFile->Draw("colz");
	hEventTimeVsFile->GetZaxis()->SetRangeUser(1,100);
}

void EmmaEventTreeReader::AnalyseRawScTimeSpectrum(){
	cout << "  <I> EmmaEventTreeReader::AnalyseRawScTimeSpectrum" << endl;
	ListPlaneCoords(vZcoord);
	int nLevels = vZcoord.size();
	TH2D* hScTime = new TH2D("absScTime", "absScTime", 55000,0,550000, nLevels,1,nLevels+1);
	hScTime->SetDirectory(0);

	// Main event loop
	int nEntries = vfs.size();
	Int_t nTotalHits = 0;
	for(int evn=0; evn<nEntries; evn++){
		TFileStorage *fs = vfs.at(evn);
		TEventAnalysis ea(&(fs->vHitPoint), &cuts);
		ea.AnalyseLevelMultiplicity(&vZcoord);
		ea.FillRawScTimeHistos(hScTime);
		nTotalHits += fs->vHitPoint.size(); // total #hits without any cuts
	}

	// Plot histo
	gStyle->SetOptStat(0);
	const string cvsName = "cRawScT";
	float lmargin = 0.07, rmargin = 0.05, tmargin = 0.01, bmargin = 0.1;
	float fontsize = 0.045;
	float xoffset = 1.0, yoffset = 0.7;
	TLegend *leg = new TLegend(0.1, 0.6, 0.63, 0.94);
	TCanvas *cAbs = new TCanvas((cvsName+"_"+fileBaseName+"_absOnly").c_str(), (cvsName+"_"+fileBaseName+"_avgOnly").c_str(), 1600,800);
	cAbs->cd();
	gPad->SetLogy();
	bool first = true;
	for(Int_t iy=1; iy<=hScTime->GetNbinsY(); iy++){
		stringstream ssname;
		ssname << hScTime->GetName() << "_nLevelsPresent=" << iy;
		TH1D* h1 = hScTime->ProjectionX(ssname.str().c_str(), iy, iy);
		h1->SetDirectory(0);
		h1->SetLineWidth(2);
		h1->SetLineColor(vCol.at(iy-1));
		if(first){
			common::SetupPad(h1, fontsize, lmargin, rmargin, tmargin, bmargin, xoffset, yoffset);
			h1->SetTitle("");
			h1->GetXaxis()->SetTitle("Time [100 ps]");
			h1->GetYaxis()->SetTitle("Number of hits");
			h1->GetXaxis()->SetRangeUser(cuts.promptT0 - 4*(cuts.promptT1-cuts.promptT0), cuts.promptT1 + 12*(cuts.promptT1-cuts.promptT0));
			h1->GetYaxis()->SetRangeUser(0.8, hScTime->GetMaximum()*2);
			h1->DrawCopy();
			first = false;
		}
		else{
			h1->DrawCopy("sames");
		}

		// Legend
		string histoName = h1->GetName();
		stringstream ssEntryName;
		ssEntryName << histoName.substr(histoName.find_first_of('_')+1);
		double nPeak = h1->Integral(h1->FindBin(cuts.promptT0), h1->FindBin(cuts.promptT1));
		ssEntryName.setf(ios::fixed);
		ssEntryName << " (" << setprecision(0) << setw(9) << right << nPeak << ", ";
		ssEntryName << setprecision(1) << 100.0*nPeak/h1->Integral(0, h1->GetNbinsX()+1) << "%, ";
		ssEntryName << setprecision(1) << 100.0*nPeak/nTotalHits << "%)";
		leg->AddEntry(h1, ssEntryName.str().c_str(), "l");
	}
	leg->Draw();

}

void EmmaEventTreeReader::AnalyseMultiplicityPerLevel(){
	cout << "  <I> EmmaEventTreeReader::AnalyseMultiplicityPerLevel()" << endl;
	vector< vector<TH2D*> > vvh2;
	vector<string> sType;
	sType.push_back("nLayers>=3");
	sType.push_back("nLayers<=2");
	sType.push_back("nLayers==3");
	sType.push_back("nLayers==4");
	sType.push_back("nLayers==5");
	ListPlaneCoords(vZcoord);
	for(UInt_t itype=0; itype<sType.size(); itype++){
		vector<TH2D*> vh2;
		for(UInt_t iz=0; iz<vZcoord.size(); iz++){
			stringstream ss;
			ss << "Level:" << iz << "(" << vZcoord.at(iz) << "mm)";
			vh2.push_back(new TH2D(ss.str().c_str(), ss.str().c_str(), 18, -125, 2125, 14, -125, 1650));
			vh2.at(iz)->SetDirectory(0);
		}
		vvh2.push_back(vh2);
	}

	TH1D* hh = new TH1D("hh","hh",1000,0,1000);
	// Main event loop
	int nEntries = vfs.size();
	for(int evn=0; evn<nEntries; evn++){
		TFileStorage *fs = vfs.at(evn);
		hh->Fill(fs->iFileNumber);
		TEventAnalysis ea(&(fs->vHitPoint), &cuts);
		ea.AnalyseLevelMultiplicity(&vZcoord);
		if(ea.getLevelsPresent() >= 3){
			ea.FillLayerHistos(vvh2.at(0));
		}
		else{
			ea.FillLayerHistos(vvh2.at(1));
		}
		if(ea.getLevelsPresent() == 3){
			ea.FillLayerHistos(vvh2.at(2));
		}
		else if(ea.getLevelsPresent() == 4){
			ea.FillLayerHistos(vvh2.at(3));
		}
		else if(ea.getLevelsPresent() == 5){
			ea.FillLayerHistos(vvh2.at(4));
		}
	}

	// Drawing
	double maxCnts = 0;

	for(UInt_t itype=0; itype<sType.size(); itype++){
		for(UInt_t iz=0; iz<vZcoord.size(); iz++){
			maxCnts = TMath::Max(maxCnts, vvh2.at(itype).at(iz)->GetMaximum());
			cout << itype << " " << iz << " " << vvh2.at(itype).at(iz)->GetMaximum() << endl;
		}
	}

	gStyle->SetOptStat(0);
	for(UInt_t itype=0; itype<sType.size(); itype++){
		stringstream ss;
		ss << "cMLayer_" << fileBaseName << "_" << sType.at(itype);
		TCanvas *c = new TCanvas(ss.str().c_str(), ss.str().c_str(), 1100, 900);
		c->Divide(2,3);
		for(UInt_t iz=0; iz<vvh2.at(itype).size(); iz++){
			if(iz<=2){
				c->cd(iz+1);
			}
			else{
				c->cd(iz+2);
			}
			vvh2.at(itype).at(iz)->GetZaxis()->SetRangeUser(1, maxCnts);
			vvh2.at(itype).at(iz)->Draw("colz");
		}
	}

	new TCanvas("cvss","cvss",1600,900);
	hh->Draw();
}

void EmmaEventTreeReader::AnalyseMultiplicityCorrelationBetweenLevels(){
	cout << "  <I> EmmaEventTreeReader::AnalyseMultiplicityCorrelationBetweenLevels()" << endl;
	vector<TH2D*> vh2;
	ListPlaneCoords(vZcoord);

	// Setup names/titles for histos and axes
	vector<string> vsZLevel;
	for(uint16_t iz=0; iz<vZcoord.size(); iz++){
		stringstream ss;
		ss << iz+1 << "(" << vZcoord.at(iz) << ")";
		vsZLevel.push_back(ss.str());
	}
	vector<string> vsHistoType;
	// The histos will contain correlation in multiplicity between level 1 and other levels
	for(uint16_t iz=1; iz<vZcoord.size(); iz++){
		vsHistoType.push_back("Level_");
		vsHistoType.at(iz-1) += vsZLevel.at(0);
		vsHistoType.at(iz-1) += "-";
		vsHistoType.at(iz-1) += vsZLevel.at(iz);
	}

	// Initialize histos
	for(UInt_t itype=0; itype<vsHistoType.size(); itype++){
		vh2.push_back(new TH2D(vsHistoType.at(itype).c_str(), vsHistoType.at(itype).c_str(), 144,0,144, 192,0,192));
//		vh2.push_back(new TH2D(vsHistoType.at(itype).c_str(), vsHistoType.at(itype).c_str(), 9,0,9, 12,0,12));
		TH2D* h2 = vh2.at(itype);
		h2->SetDirectory(0);
		h2->SetXTitle(vsZLevel.at(0).c_str());
		h2->SetYTitle(vsZLevel.at(itype+1).c_str());
	}
	TH2D* hComplex = new TH2D("h_BOT+TOP_vs_BOT-TOP","BOT+TOP_vs_BOT-TOP", 100,0,100, 100*2,-100,100);
//	TH2D* hComplex = new TH2D("h_BOT+TOP_vs_BOT-TOP","BOT+TOP_vs_BOT-TOP", 21,0,21, 12*2,-12,12);
	TH1D* hDiv = new TH1D("hDiv","hDiv", 9,-1.125,1.125);
	TH1D* hDivRnd = new TH1D("hDivRnd","hDivRnd", 9,-1.125,1.125);
	TH2D* hComplexRnd = new TH2D("hRnd_BOT+TOP_vs_BOT-TOP","Rnd_BOT+TOP_vs_BOT-TOP", 100,0,100, 100*2,-100,100);
	TH1D* hMultiDistr = new TH1D("hMultiDistr","hMultiDistr",50,0,50);
	TH2D* hCorrRnd = new TH2D("hCorrRnd","hCorrRnd",50,0,50,50,0,50);

	// Main event loop
	int nEntries = vfs.size();
	for(int evn=0; evn<nEntries; evn++){
		TFileStorage *fs = vfs.at(evn);
		TEventAnalysis ea(&(fs->vHitPoint), &cuts);
/*		ea.AnalyseLevelSc16Multiplicity(&vZcoord);
 		for(UInt_t itype=0; itype<vh2.size(); itype++){
			vh2.at(itype)->Fill(ea.getLevelSc16Multiplicity().at(0), ea.getLevelSc16Multiplicity().at(itype+1));
		}
		if((ea.getLevelSc16Multiplicity().at(0) >= 0) && (ea.getLevelSc16Multiplicity().at(4) >= 0)){
			hComplex->Fill(ea.getLevelSc16Multiplicity().at(0) + ea.getLevelSc16Multiplicity().at(4), ea.getLevelSc16Multiplicity().at(0) - ea.getLevelSc16Multiplicity().at(4));
		}
*/
		ea.AnalyseLevelMultiplicity(&vZcoord);
 		for(UInt_t itype=0; itype<vh2.size(); itype++){
			vh2.at(itype)->Fill(ea.getLevelMultiplicity().at(0), ea.getLevelMultiplicity().at(itype+1));
		}
		if((ea.getLevelMultiplicity().at(0) >= 0) && (ea.getLevelMultiplicity().at(4) >= 0)){
			hComplex->Fill(ea.getLevelMultiplicity().at(0) + ea.getLevelMultiplicity().at(4), ea.getLevelMultiplicity().at(0) - ea.getLevelMultiplicity().at(4));
		}
		if((ea.getLevelMultiplicity().at(0) != 0) || (ea.getLevelMultiplicity().at(4) != 0)){
			float complex = ((float)ea.getLevelMultiplicity().at(0) - (float)ea.getLevelMultiplicity().at(4)) / ((float)ea.getLevelMultiplicity().at(0) + (float)ea.getLevelMultiplicity().at(4));
			hDiv->Fill(complex);
		}


		// Playing with random numbers
		int rndTop = round(gRandom->Exp(2.5));
		int rndBot = round(gRandom->Exp(2.5));
		hMultiDistr->Fill(rndTop);
		hMultiDistr->Fill(rndBot);
		if((rndTop >= 0) && (rndBot >= 0)){
			hComplexRnd->Fill(rndBot + rndTop, rndBot - rndTop);
		}
		if((rndTop != 0) || (rndBot != 0)){
			float complex = ((float)rndTop-(float)rndBot) / ((float)rndTop+(float)rndBot);
			hDivRnd->Fill(complex);
			if(evn<1e4){
				cout << rndTop << " " << rndTop << " " << complex << endl;
			}
		}
		hCorrRnd->Fill(rndTop,rndBot);
	}

	// Drawing
	double maxCnts = 0;
	for(UInt_t itype=0; itype<vh2.size(); itype++){
		maxCnts = TMath::Max(maxCnts, vh2.at(itype)->GetMaximum());
		cout << itype << " " << vh2.at(itype)->GetName() << " " << vh2.at(itype)->GetMaximum() << endl;
	}

	//gStyle->SetOptStat(0);
	TCanvas *c = new TCanvas("MultiplicityCorr", "MultiplicityCorr", 1600, 900);
	c->Divide(2,2);
	for(UInt_t itype=0; itype<vh2.size(); itype++){
		c->cd(itype+1);
		vh2.at(itype)->GetZaxis()->SetRangeUser(1, maxCnts);
		vh2.at(itype)->Draw("colz");
		gPad->SetLogz();
	}

	TCanvas *cComplex = new TCanvas("cComplex", "BOT+TOP_vs_BOT-TOP", 1600, 700);
	cComplex->Divide(2);
	cComplex->cd(1);
	gPad->SetLogz();
	hComplex->Draw("colz");
	hComplex->SetTitle("");
	hComplex->SetXTitle("Multiplicity(BOT+TOP)");
	hComplex->SetYTitle("Multiplicity(BOT-TOP)");
	cComplex->cd(2);
	gPad->SetLogz();
	hComplexRnd->Draw("colz");
	hComplexRnd->SetTitle("");
	hComplexRnd->SetXTitle("Multiplicity(BOT+TOP)");
	hComplexRnd->SetYTitle("Multiplicity(BOT-TOP)");

	TCanvas *cDiv = new TCanvas("cDiv", "BOT-TOP_/_BOT+TOP", 1600, 900);
	cDiv->cd();
	gPad->SetLogy();
	hDiv->Draw("colz");
	hDiv->SetTitle("");
	hDiv->SetLineWidth(2);
	hDiv->SetLineColor(kBlack);
	hDiv->SetYTitle("(M_{BOT}-M_{TOP}) / (M_{BOT}+M_{TOP})");
	hDivRnd->SetLineWidth(2);
	hDivRnd->SetLineColor(kRed);
	hDivRnd->Draw("sames");

	TCanvas *cMultiDistr = new TCanvas("cMultiDistr", "cMultiDistr", 1600, 900);
	cMultiDistr->cd();
	gPad->SetLogy();
	hMultiDistr->SetLineWidth(2);
	hMultiDistr->SetLineColor(kBlack);
	hMultiDistr->Draw();

	TCanvas *cCorrRnd = new TCanvas("cCorrRnd", "cCorrRnd", 1600, 900);
	cCorrRnd->cd();
	gPad->SetLogz();
	hCorrRnd->Draw("colz");
}

void EmmaEventTreeReader::AnalyseTimingBetweenLevels(){
	cout << "  <I> EmmaEventTreeReader::AnalyseTimingBetweenLevels()" << endl;
	gStyle->SetOptStat("nemr");
	vector<TH1D*> vhTimingPerLayer;
	vector<TH1D*> vhAvgTimeDiffBetweenLayers;
	ListPlaneCoords(vZcoord);
	vector<double> vAvgTimePerLayer(vZcoord.size(),0);
	// Setup histo names
	int datawidth = cuts.promptT1-cuts.promptT0;
	int bin0 = cuts.promptT0 - 0.2*datawidth;
	int bin1 = cuts.promptT1 + 0.2*datawidth;
	for(uint16_t iz=0; iz<vZcoord.size(); iz++){
		stringstream ss;
		ss << "Timing_at_Level-" << iz+1 << "-" << vZcoord.at(iz);
		vhTimingPerLayer.push_back(new TH1D(ss.str().c_str(), ss.str().c_str(), bin1-bin0, bin0, bin1));
		TH1D* h1 = vhTimingPerLayer.at(iz);
		h1->SetDirectory(0);
		h1->SetLineWidth(2);
		h1->SetLineColor(vCol.at(iz));
		h1->SetXTitle("Raw TDC time [100 ps]");
		if(iz<4){
			stringstream ss2;
			ss2 << "dt(L5" << "-L" << iz+1 << ")";
			vhAvgTimeDiffBetweenLayers.push_back(new TH1D(ss2.str().c_str(), ss2.str().c_str(), 2*datawidth, -datawidth, datawidth));
			TH1D* h = vhAvgTimeDiffBetweenLayers.at(iz);
			h->SetDirectory(0);
			h->SetLineWidth(2);
			h->SetLineColor(vCol.at(iz));
			h->SetXTitle("Time difference [100 ps]");
		}
	}

	// Main event loop
	int nEntries = vfs.size();
	for(int evn=0; evn<nEntries; evn++){
		TFileStorage *fs = vfs.at(evn);
		TEventAnalysis ea(&(fs->vHitPoint), &cuts);
		ea.AnalyseLevelMultiplicity(&vZcoord);
		for(uint16_t iz=0; iz<vZcoord.size(); iz++){
			vAvgTimePerLayer.at(iz) = ea.FillAvgHitTime(vhTimingPerLayer.at(iz), vZcoord.at(iz));
		}
		for(uint16_t iz=0; iz<vZcoord.size()-1; iz++){
			if((vAvgTimePerLayer.at(4) > 1e-3) && (vAvgTimePerLayer.at(iz) > 1e-3)){
				vhAvgTimeDiffBetweenLayers.at(iz)->Fill(vAvgTimePerLayer.at(4)-vAvgTimePerLayer.at(iz));
			}
		}
	}

	// Drawing avg time distribution per layer
	double maxCnts = 0;
	for(UInt_t ih=0; ih<vhTimingPerLayer.size(); ih++){
		maxCnts = TMath::Max(maxCnts, vhTimingPerLayer.at(ih)->GetMaximum());
	}
	TCanvas *cAvg = new TCanvas("AvgTimePerLayer", "AvgTimePerLayer", 1600, 900);
	cAvg->cd();
	gPad->SetLogy();
	gPad->SetGrid(1,1);
	for(UInt_t ih=0; ih<vhTimingPerLayer.size(); ih++){
		TH1D* h = vhTimingPerLayer.at(ih);
		h->GetYaxis()->SetRangeUser(1, maxCnts*1.1);
		h->Draw(ih==0?"":"sames");
		gPad->Update();
		common::ArrangeStatBox(h,ih,1.0,1.0,0.16,0.22);
	}

	// Drawing avg time difference between layers
	for(UInt_t ih=0; ih<vhAvgTimeDiffBetweenLayers.size(); ih++){
		maxCnts = TMath::Max(maxCnts, vhAvgTimeDiffBetweenLayers.at(ih)->GetMaximum());
	}
	TCanvas *cDiff = new TCanvas("TimeDiff", "TimeDiff", 1600, 900);
	cDiff->cd();
	gPad->SetLogy();
	gPad->SetGrid(1,1);
	for(UInt_t ih=0; ih<vhAvgTimeDiffBetweenLayers.size(); ih++){
		TH1D* h = vhAvgTimeDiffBetweenLayers.at(ih);
		h->GetYaxis()->SetRangeUser(1, maxCnts*1.1);
		h->Draw(ih==0?"":"sames");
		gPad->Update();
		common::ArrangeStatBox(h,ih,1.0,1.0,0.18,0.22);
	}
}

void EmmaEventTreeReader::setIgnoreHitsWithoutPattern(bool ignoreHitsWithoutPattern) {
	cuts.ignoreHitsWithoutPattern = ignoreHitsWithoutPattern;
}

void EmmaEventTreeReader::setIgnoreHitsWithoutTiming(bool ignoreHitsWithoutTiming) {
	cuts.ignoreHitsWithoutTiming = ignoreHitsWithoutTiming;
}

void EmmaEventTreeReader::setAcceptHitsFromPromptPeakOnly(bool acceptHitsFromPromptPeakOnly, int t0, int t1){
	cuts.acceptHitsFromPromptPeakOnly = acceptHitsFromPromptPeakOnly;
	cuts.promptT0 = t0;
	cuts.promptT1 = t1;
}

void EmmaEventTreeReader::FilterOutBadFiles(vector<int> &viFile){
	cout << "  <I> EmmaEventTreeReader::FilterOutBadFiles(): ";
	for(UInt_t i=0; i<viFile.size(); i++){
		cout << viFile.at(i) << " ";
	}
	cout << endl;

	int nEntries = vfs.size();
	for(UInt_t i=0; i<viFile.size(); i++){
		int evn0 = 0, evn1 = 0;
		float first = true;
		for(int evn=0; evn<nEntries; evn++){
			TFileStorage *fs = vfs.at(evn);
			if(fs->iFileNumber == viFile.at(i)){
				if(first){
					evn0 = evn;
					first = false;
				}
			}
			else{
				if(!first){
					evn1 = evn;
					break;
				}
			}
		}
		vfs.erase(vfs.begin() + evn0, vfs.begin() + evn1);
	}
}



// ---------------------------
// ----- Private methods -----
// ---------------------------
void EmmaEventTreeReader::ListPlaneCoords(vector<int16_t>& vout, Int_t evnLimit){
	vector<int16_t> v;
	vout.clear();

	int nEntries = vfs.size();
	if(nEntries > evnLimit){
		nEntries = evnLimit;
	}
	for(int evn=0; evn<nEntries; evn++){
		vector<THitStorage> *vHit = &vfs.at(evn)->vHitPoint;
		for(UInt_t ihit=0; ihit<vHit->size(); ihit++){
			v.push_back(vHit->at(ihit).z);
		}
	}
	sort(v.begin(), v.end());

	//remove closeby coordinates, because they belong to the same plane
	const float SAME_PLANE_EPSILON = 1;
	int16_t prevv=v.at(0);
	vout.push_back(prevv);
	for(uint16_t i=1; i<v.size(); i++){
		if(fabs(v.at(i)-prevv) > SAME_PLANE_EPSILON){
			vout.push_back(v.at(i));
		}
		prevv = v.at(i);
	}
}

} /* namespace std */
