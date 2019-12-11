/*
 * emmaEventTreeReader.cpp
 *
 *  Created on: Nov 18, 2019
 *      Author: mss
 */

#include "EmmaEventTreeReader.h"

namespace std {

EmmaEventTreeReader::EmmaEventTreeReader() {
}

EmmaEventTreeReader::~EmmaEventTreeReader(){
}

int EmmaEventTreeReader::ReadTreeFromRootFile(const string &filePath, const string &treeName, const string &objectName){
	this->filePath = filePath;
	fileBaseName = filePath.substr(filePath.find_last_of('/')+1);
	fileBaseName = fileBaseName.substr(0, fileBaseName.find_last_of('.'));

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

void EmmaEventTreeReader::AnalyseEventTimeSpectrum(){
	cout << "  <I> EmmaEventTreeReader::AnalyseEventTimeSpectrum" << endl;
	string hName = "eventTimeDiff";
	TH1D* hEventTime = new TH1D(hName.c_str(), hName.c_str(), 1e6,0,8000);
	hEventTime->SetDirectory(0);

	// Main event loop
	int nEntries = vfs.size();
	Double_t prevEventTime = 0;
	for(int evn=0; evn<nEntries; evn++){
		TFileStorage *fs = vfs.at(evn);
		TEventAnalysis ea(&(fs->vHitPoint), &cuts);
		if(evn>0){
			hEventTime->Fill((fs->fEventTimeS-prevEventTime)*1e3);
		}
		prevEventTime = fs->fEventTimeS;
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
}

void EmmaEventTreeReader::AnalyseRawScTimeSpectrum(){
	cout << "  <I> EmmaEventTreeReader::AnalyseRawScTimeSpectrum" << endl;
	vector<TH2D*> vhScTime;
	vhScTime.push_back(new TH2D("absScTime"    ,"absScTime"    , 55000,0,550000, 4,1,5));
	vhScTime.push_back(new TH2D("avgScTime"    ,"avgScTime"    , 55000,0,550000, 4,1,5));
	vhScTime.push_back(new TH2D("rmsScTime"    ,"rmsScTime"    , 55000,0,550000, 4,1,5));
	for(UInt_t itype=0; itype<vhScTime.size(); itype++){
		vhScTime.at(itype)->SetDirectory(0);
	}

	// Main event loop
	ListPlaneCoords(vZcoord);
	int nEntries = vfs.size();
	for(int evn=0; evn<nEntries; evn++){
		TFileStorage *fs = vfs.at(evn);
		TEventAnalysis ea(&(fs->vHitPoint), &cuts);
		ea.AnalyseLevelMultiplicity(&vZcoord);
		ea.FillRawScTimeHistos(vhScTime);
	}

	// Plot histo
	vector<Color_t> vCol;
	vCol.push_back(kRed);
	vCol.push_back(kMagenta);
	vCol.push_back(kBlue);
	vCol.push_back(kBlack);
	const string cvsName = "cRawScT";
	float lmargin = 0.053, rmargin = 0.038, tmargin = 0.01, bmargin = 0.22;
	float fontsize = 0.1;
	float xoffset = 0.85, yoffset = 0.24;
	gStyle->SetOptStat(0);
	TCanvas *c = new TCanvas((cvsName+"_"+fileBaseName).c_str(), (cvsName+"_"+fileBaseName).c_str(), 1600,800);
	c->Divide(1,vhScTime.size());
	float ymax = 0;
	for(UInt_t ih=0; ih<vhScTime.size(); ih++){
		TH2D* h2 = vhScTime.at(ih);
		if(ymax < h2->GetMaximum()){
			ymax = h2->GetMaximum();
		}
	}
	TLegend *leg = new TLegend(0.75, bmargin+0.05, 0.95, 1.0-tmargin-0.05);
	vector<TH1D*> vhAbs;
	for(UInt_t ih=0; ih<vhScTime.size(); ih++){
		c->cd(ih+1);
		TH2D* h2 = vhScTime.at(ih);
		gPad->SetLogy();
		bool first = true;
		for(Int_t iy=1; iy<=h2->GetNbinsY(); iy++){
			stringstream ssname;
			ssname << h2->GetName() << "_nLevelsPresent=" << iy;
			TH1D* h1 = h2->ProjectionX(ssname.str().c_str(), iy, iy);
			h1->SetDirectory(0);
			h1->SetLineWidth(2);
			h1->SetLineColor(vCol.at(iy-1));
			if(first){
				common::SetupPad(h1, fontsize, lmargin, rmargin, tmargin, bmargin, xoffset, yoffset);
				h1->SetTitle("");
				h1->DrawCopy();
				h1->GetXaxis()->SetTitle("Time [100 ps]");
				h1->GetYaxis()->SetTitle("Number of hits");
				h1->GetYaxis()->SetRangeUser(0.8, ymax*5);
				string padName = h1->GetName();
				padName = padName.substr(0, padName.find_first_of('_'));
				common::DrawTextNdc(padName.c_str(), 0.08, 0.85, fontsize);
				first = false;
			}
			else{
				h1->DrawCopy("sames");
			}
			if(ih == vhScTime.size()-1){
				string legName = h1->GetName();
				legName = legName.substr(legName.find_first_of('_')+1);
				leg->AddEntry(h1, legName.c_str(), "l");
				leg->Draw();
			}

			if(ih==0){
				vhAbs.push_back(h1);
			}
		}
	}

	TCanvas *cAvg = new TCanvas((cvsName+"_"+fileBaseName+"_absOnly").c_str(), (cvsName+"_"+fileBaseName+"_avgOnly").c_str(), 1600,800);
	cAvg->cd();
	gPad->SetLogy();
	TH1D* h = vhAbs.at(0);
	common::SetupPad(h, 0.045, 0.07, 0.05, 0.01, 0.1, 1.0, 0.7);
	h->Draw();
	h->GetXaxis()->SetRangeUser(cuts.promptT0 - 4*(cuts.promptT1-cuts.promptT0), cuts.promptT1 + 12*(cuts.promptT1-cuts.promptT0));
	for(UInt_t ih=1; ih<vhAbs.size(); ih++){
		vhAbs.at(ih)->Draw("same");
	}

	// Find total number of hits irrespective of any cuts
	Int_t nTotalHits = 0;
	for(int evn=0; evn<nEntries; evn++){
		TFileStorage *fs = vfs.at(evn);
		nTotalHits += fs->vHitPoint.size();
	}

	TLegend *leg2 = new TLegend(0.4, 0.6, 0.93, 0.94);
	for(UInt_t ih=0; ih<vhAbs.size(); ih++){
		TH1D* h = vhAbs.at(ih);
		string histoName = h->GetName();
		stringstream ssEntryName;
		ssEntryName << histoName.substr(histoName.find_first_of('_')+1);
		double nPeak = h->Integral(h->FindBin(cuts.promptT0), h->FindBin(cuts.promptT1));
		ssEntryName.setf(ios::fixed);
		ssEntryName << " (" << setprecision(0) << setw(9) << right << nPeak << ", ";
		ssEntryName << setprecision(1) << 100.0*nPeak/h->Integral(0, h->GetNbinsX()+1) << "%, ";
		ssEntryName << setprecision(1) << 100.0*nPeak/nTotalHits << "%)";
		leg2->AddEntry(h, ssEntryName.str().c_str(), "l");
	}
	leg2->Draw();
}

void EmmaEventTreeReader::AnalyseMultiplicityPerLevel(){
	cout << "  <I> EmmaEventTreeReader::AnalyseMultiplicityPerLevel()" << endl;
	vector< vector<TH2D*> > vvh2;
	vector<string> sType;
	sType.push_back("nLayers>=3");
	sType.push_back("nLayers<=2");
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
		TCanvas *c = new TCanvas(ss.str().c_str(), ss.str().c_str(), 1600, 900);
		c->Divide(2,2);
		for(UInt_t iz=0; iz<vvh2.at(itype).size(); iz++){
			c->cd(iz+1);
			vvh2.at(itype).at(iz)->GetZaxis()->SetRangeUser(1, maxCnts);
			vvh2.at(itype).at(iz)->Draw("colz");
		}
	}

	new TCanvas("cvss","cvss",1600,900);
	hh->Draw();
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
