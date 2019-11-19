/*
 * emmaEventTreeReader.cpp
 *
 *  Created on: Nov 18, 2019
 *      Author: mss
 */

#include "EmmaEventTreeReader.h"

namespace std {

EmmaEventTreeReader::EmmaEventTreeReader() {
	f = nullptr;
	t = nullptr;
	fs = nullptr;
}

EmmaEventTreeReader::~EmmaEventTreeReader(){
	if(f != nullptr){
		f->Close();
	}
}

int EmmaEventTreeReader::OpenRootFile(const string &filePath){
	this->filePath = filePath;
	fileBaseName = filePath.substr(filePath.find_last_of('/')+1);
	fileBaseName = fileBaseName.substr(0, fileBaseName.find_last_of('.'));

	cout << "  <I> Opening file: " << filePath << endl;
	f = new TFile(filePath.c_str(), "READ"); // try opening the root file
	if(f==NULL){
		cout << "<E> Problems opening file: " << filePath << endl;
		return -1; // exit program
	}
	return 0;
}

int EmmaEventTreeReader::ReadTree(const string &treeName){
	t = (TTree*)(f->Get(treeName.c_str())); // get the tree from file
	if(t==NULL){
		cout << "<E> Problems opening tree: " << treeName << endl;
		return -1; // exit program
	}
	return 0;
}

int EmmaEventTreeReader::ReadFileStorage(const string &objectName){
	t->SetBranchAddress(objectName.c_str(), &fs);
	if(fs==NULL){
		cout << "<E> Problems opening event object: " << objectName << endl;
		return -1; // exit program
	}
	return 0;
}

void EmmaEventTreeReader::PrintStorageContents(ostream &out, uint64_t evn){
	int nentries = t->GetEntries();
	if(evn<nentries){
		t->GetEntry(evn);
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

	int nentries = t->GetEntries();
	for(int evn=0; evn<nentries; evn++){
		for(int isc=0; isc<nSc; isc++){
			vTimePresent.at(isc) = false;
			vHbPresent.at(isc) = false;
		}

		t->GetEntry(evn);
		for(UInt_t ihit=0; ihit<fs->vHitPoint.size(); ihit++){
			THitStorage *hs = &(fs->vHitPoint.at(ihit));
			if(hs->t != 0){
				vTimePresent.at(hs->scModule) = true;
			}
			if(hs->scPixel >= 0){
				vHbPresent.at(hs->scModule) = true;
			}
		}

		for(int isc=0; isc<nSc; isc++){
			if(vTimePresent.at(isc) || vHbPresent.at(isc)){
				vTimeOrHbCnt.at(isc)++;
			}
			if(vTimePresent.at(isc)){
				vTimeCnt.at(isc)++;
				if(vHbPresent.at(isc)){
					vTimeAndHbCnt.at(isc)++;
				}
			}
			if(vHbPresent.at(isc)){
				vHbCnt.at(isc)++;
			}
		}
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
			h->GetXaxis()->SetTitle("SC number");
			common::SetupPad(h, fontsize, 0.07, 0.01, 0.01, 0.14, 1.0, 0.55);
		}
		else{
			h->Draw("same");
		}
		common::DrawTextNdc(vhName.at(ih), 0.15, 0.4-fontsize*ih, fontsize, h->GetLineColor());
	}
}

void EmmaEventTreeReader::AnalyseMultiplicityPerLevel(){
	vector<int16_t> vZcoord;
	ListPlaneCoords(vZcoord);
	vector<TH2D*> vh2;
	const float dxyPixel = 0.5 * (PIXEL_SIZE + PIXEL_GAP);
	for(UInt_t iz=0; iz<vZcoord.size(); iz++){
		stringstream ss;
		ss << "Level:" << iz << "(" << vZcoord.at(iz) << "mm)";
		vh2.push_back(new TH2D(ss.str().c_str(), ss.str().c_str(), 18, -125, 2125, 14, -125, 1650));
		vh2.at(iz)->SetDirectory(0);
	}
	int nentries = t->GetEntries();
	for(int evn=0; evn<nentries; evn++){
		t->GetEntry(evn);
		vector<THitStorage> *vHit = &(fs->vHitPoint);

		// Check if event contains at least one hit in each layer
		vector<int> vLevelMultiplicity(vZcoord.size(), 0);
		for(UInt_t ihit=0; ihit<vHit->size(); ihit++){
			THitStorage* hit = &(vHit->at(ihit));
			for(UInt_t iz=0; iz<vZcoord.size(); iz++){
				if(vZcoord.at(iz) == hit->z){
					vLevelMultiplicity.at(iz)++;
				}
			}
		}
		UInt_t nLevelsPresent = 0;
		for(UInt_t iz=0; iz<vLevelMultiplicity.size(); iz++){
			if(vLevelMultiplicity.at(iz) > 0){
				nLevelsPresent++;
			}
		}

		if(nLevelsPresent == 3){
			for(UInt_t ihit=0; ihit<vHit->size(); ihit++){
				THitStorage* hit = &(vHit->at(ihit));
				for(UInt_t iz=0; iz<vZcoord.size(); iz++){
					if(vZcoord.at(iz) == hit->z){
						if(hit->scPixel == -1){ // if no pattern info present: paint all 16 pixels with 1/16th weight
							for(UInt_t ipixel=0; ipixel<N_PIXELS; ipixel++){
								float dx = dxyPixel*(2. * (ipixel%4) - 3);
								float dy = dxyPixel*(2. * (ipixel/4) - 3);
								vh2.at(iz)->Fill(hit->x+dx, hit->y+dy, 1./N_PIXELS);
							}
						}
						else{
							vh2.at(iz)->Fill(hit->x, hit->y);
						}
					}
				}
			}
		}
	}

	double maxCnts = 0;
	for(UInt_t iz=0; iz<vh2.size(); iz++){
		maxCnts = TMath::Max(maxCnts, vh2.at(iz)->GetMaximum());
	}

	// Drawing
	gStyle->SetOptStat(0);
	string sCvsName = "cCnt_" + fileBaseName;
	TCanvas *c = new TCanvas(sCvsName.c_str(), sCvsName.c_str(), 1600, 900);
	c->Divide(2,2);
	for(UInt_t iz=0; iz<vh2.size(); iz++){
		c->cd(iz+1);
		vh2.at(iz)->GetZaxis()->SetRangeUser(0, maxCnts);
		vh2.at(iz)->Draw("colz");
	}
}




// ---------------------------
// ----- Private methods -----
// ---------------------------
void EmmaEventTreeReader::ListPlaneCoords(vector<int16_t>& vout, UInt_t evnLimit){
	vector<int16_t> v;
	vout.clear();
	bool exists = false;

	int nentries = t->GetEntries();
	if(nentries > evnLimit){
		nentries = evnLimit;
	}
	for(int evn=0; evn<nentries; evn++){
		t->GetEntry(evn);
		vector<THitStorage> *vHit = &(fs->vHitPoint);
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
