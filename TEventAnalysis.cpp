/*
 * TEventAnalysis.cpp
 *
 *  Created on: Dec 3, 2019
 *      Author: mss
 */

#include <math.h>
#include "TEventAnalysis.h"
#include "TRandom3.h"
#include "common.h"

namespace std {

TEventAnalysis::TEventAnalysis(TFileStorage *event, TCuts *cuts, vector<double> *vZcoord) {
	this->event = event;
	this->cuts = cuts;
	this->vZcoord = vZcoord;
	nLevelsPresent = 0;
	deletePointers = false;
	vHit = &(event->vHitPoint);
}

TEventAnalysis::TEventAnalysis(TEventAnalysis &ea){
	event = new TFileStorage(*ea.event);
	nLevelsPresent = ea.getLevelsPresent();
	cuts = new TCuts(ea.getCuts());
	vZcoord = new vector<double>;
	for(UInt_t i=0; i<ea.getZcoord().size(); i++){
		vZcoord->push_back(ea.getZcoord().at(i));
	}
	for(UInt_t i=0; i<ea.getLevelPixelMultiplicity().size(); i++){
		vLevelPixelMultiplicity.push_back(ea.getLevelPixelMultiplicity().at(i));
	}
	for(UInt_t i=0; i<ea.getLevelSc16Multiplicity().size(); i++){
		vLevelSc16Multiplicity.push_back(ea.getLevelSc16Multiplicity().at(i));
	}
	for(UInt_t i=0; i<ea.getAvgX().size(); i++){
		vAvgX.push_back(ea.getAvgX().at(i));
	}
	for(UInt_t i=0; i<ea.getAvgY().size(); i++){
		vAvgY.push_back(ea.getAvgY().at(i));
	}
	for(UInt_t i=0; i<ea.getSigma2X().size(); i++){
		vSigma2X.push_back(ea.getSigma2X().at(i));
	}
	for(UInt_t i=0; i<ea.getSigma2Y().size(); i++){
		vSigma2Y.push_back(ea.getSigma2Y().at(i));
	}
	deletePointers = true;
	vHit = &(event->vHitPoint);
}

TEventAnalysis::~TEventAnalysis() {
	if(deletePointers){
		delete event;
		delete cuts;
		delete vZcoord;
	}
}

void TEventAnalysis::AnalyseLevelMultiplicity(){
	// Enlarge the vectors to have the size of number of layers
	if(vAvgX.size() != 0){
		vLevelPixelMultiplicity.erase(vLevelPixelMultiplicity.begin(), vLevelPixelMultiplicity.end());
		vAvgX.erase(vAvgX.begin(), vAvgX.end());
		vAvgY.erase(vAvgY.begin(), vAvgY.end());
		vSigma2X.erase(vSigma2X.begin(), vSigma2X.end());
		vSigma2Y.erase(vSigma2Y.begin(), vSigma2Y.end());
	}
	vLevelPixelMultiplicity.insert(vLevelPixelMultiplicity.end(), vZcoord->size(), 0);
	vAvgX.insert(vAvgX.end(), vZcoord->size(), 0);
	vAvgY.insert(vAvgY.end(), vZcoord->size(), 0);
	vSigma2X.insert(vSigma2X.end(), vZcoord->size(), 0);
	vSigma2Y.insert(vSigma2Y.end(), vZcoord->size(), 0);
	for(UInt_t iz=0; iz<vZcoord->size(); iz++){
		for(UInt_t ihit=0; ihit<vHit->size(); ihit++){
			THitStorage* hit = &(vHit->at(ihit));
			if(isWithinCuts(hit)){
				if(common::AreSame(vZcoord->at(iz), hit->z, EPSILON)){
					vLevelPixelMultiplicity.at(iz)++;
					vAvgX.at(iz) += hit->x;
					vAvgY.at(iz) += hit->y;
				}
			}
		}
		if(vLevelPixelMultiplicity.at(iz) != 0){
			vAvgX.at(iz) /= vLevelPixelMultiplicity.at(iz);
			vAvgY.at(iz) /= vLevelPixelMultiplicity.at(iz);
		}
		for(UInt_t ihit=0; ihit<vHit->size(); ihit++){
			THitStorage* hit = &(vHit->at(ihit));
			if(isWithinCuts(hit)){
				if(common::AreSame(vZcoord->at(iz), hit->z, EPSILON)){
					vSigma2X.at(iz) += pow(vAvgX.at(iz) - hit->x, 2);
					vSigma2Y.at(iz) += pow(vAvgY.at(iz) - hit->y, 2);
				}
			}
		}
	}

	nLevelsPresent = 0;
	for(UInt_t iz=0; iz<vLevelPixelMultiplicity.size(); iz++){
		if(vLevelPixelMultiplicity.at(iz) > 0){
			nLevelsPresent++;
		}
	}
}

void TEventAnalysis::AnalyseLevelSc16Multiplicity(){
	// Check number of hits per layer
	vLevelSc16Multiplicity.insert(vLevelSc16Multiplicity.end(), vZcoord->size(), 0);
	vector<Int_t> vScModulesPresent;
	for(UInt_t ihit=0; ihit<vHit->size(); ihit++){
		THitStorage* hit = &(vHit->at(ihit));
		bool skipHit = false;
		if(ihit == 0){
			vScModulesPresent.push_back(hit->scModule);
		}
		else{
			// Check if this hit belongs to the SC16, that was already taken into account
			for(UInt_t isc=0; isc<vScModulesPresent.size(); isc++){
				if(vScModulesPresent.at(isc) == hit->scModule){
					skipHit = true;
				}
			}
		}
		if(skipHit){
			continue;
		}
		else{
			vScModulesPresent.push_back(hit->scModule);
		}

		if(isWithinCuts(hit)){
			for(UInt_t iz=0; iz<vZcoord->size(); iz++){
				if(common::AreSame(vZcoord->at(iz), hit->z, EPSILON)){
					vLevelSc16Multiplicity.at(iz)++;
				}
			}
		}
	}
	for(UInt_t iz=0; iz<vLevelSc16Multiplicity.size(); iz++){
		if(vLevelSc16Multiplicity.at(iz) > 0){
			nLevelsPresent++;
		}
	}
}

void TEventAnalysis::FillLayerHistos(vector<TH2D*> &vh2){
	if(vZcoord->size() < 1){
		cout << " <W> TEventAnalysis::FillLayerHistos(): uninitialized vZcoord; run AnalyseLevelMultiplicity() beforehand." << endl;
	}
	for(UInt_t ihit=0; ihit<vHit->size(); ihit++){
		THitStorage* hit = &(vHit->at(ihit));
		if(isWithinCuts(hit)){
			for(UInt_t iz=0; iz<vZcoord->size(); iz++){
				if(common::AreSame(vZcoord->at(iz), hit->z, EPSILON)){
					if(hit->scPixel == -1){ // if no pattern info present: paint all 16 pixels with 1/16th weight
						for(UInt_t ipixel=0; ipixel<N_PIXELS_PER_SCM; ipixel++){
							float dx = DXY_PIXEL*(2. * (ipixel%4) - 3);
							float dy = DXY_PIXEL*(2. * (ipixel/4) - 3);
							vh2.at(iz)->Fill(hit->x+dx, hit->y+dy, 1./N_PIXELS_PER_SCM);
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

void TEventAnalysis::FillHbTbHistos(vector<int> &vTimeCnt, vector<int> &vHbCnt, vector<int> &vTimeAndHbCnt, vector<int> &vTimeOrHbCnt){
	vector<bool> vTimePresent(vTimeCnt.size(),false);
	vector<bool> vHbPresent(vTimeCnt.size(),false);

	for(UInt_t ihit=0; ihit<vHit->size(); ihit++){
		THitStorage *hit = &(vHit->at(ihit));
		if(isTimePresent(hit)){
			vTimePresent.at(hit->scModule) = true;
		}
		if(isPatternPresent(hit)){
			vHbPresent.at(hit->scModule) = true;
		}
	}

	for(UInt_t isc=0; isc<vTimeCnt.size(); isc++){
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

void TEventAnalysis::FillRawScTimeHistos(TH2D* h2){
	for(UInt_t ihit=0; ihit<vHit->size(); ihit++){
		THitStorage* hit = &(vHit->at(ihit));
		if(isWithinCuts(hit)){
			if(hit->t > EPSILON){
				h2->Fill(hit->t, nLevelsPresent);
			}
		}
	}
}

double TEventAnalysis::FillAvgHitTime(TH1D* h, int16_t zCoord){
	double avgT = 0;
	UInt_t nT = 0;
	for(UInt_t ihit=0; ihit<vHit->size(); ihit++){
		THitStorage* hit = &(vHit->at(ihit));
		if(isWithinCuts(hit)){
			if((hit->z == zCoord) && (hit->t > EPSILON)){
				avgT += hit->t;
				nT++;
			}
		}
	}
	avgT /= nT;
	h->Fill(avgT);
	return avgT;
}

void TEventAnalysis::FillHitPosLevel(vector<TH2D*> &vh2control, vector<TH2D*> &vh2, int16_t multiplicityMin, int16_t multiplicityMax, float sigmaMin, float sigmaMax){
	bool storeAllLayers = false;
	for(UInt_t iz=0; iz<vLevelPixelMultiplicity.size(); iz++){
		// Multiplicity cut
		if(common::IsBetween(vLevelPixelMultiplicity.at(iz), -0.1+multiplicityMin, 0.1+multiplicityMax)){
			storeAllLayers = true;
			break;
		}
	}
	for(UInt_t iz=0; iz<vLevelPixelMultiplicity.size(); iz++){
		// Multiplicity cut
		//if(storeAllLayers){
		if(common::IsBetween(vLevelPixelMultiplicity.at(iz), -0.1+multiplicityMin, 0.1+multiplicityMax)){
			double spread = TMath::Sqrt(vSigma2X.at(iz) + vSigma2Y.at(iz));
			// Spread cut
			if(common::IsBetween(spread, sigmaMin, sigmaMax)){
				vh2control.at(0)->Fill(spread, vLevelPixelMultiplicity.at(iz));
				if(vh2.size() > 100){ // don't store more than 100 histos
					return;
				}
				stringstream ss;
				ss << event->iEventNumberWithinRun << "_" << "L" << iz;
				TH2D* h2xy = new TH2D(ss.str().c_str(), ss.str().c_str(), 18, -125, 2125, 14, -125, 1650);
				for(UInt_t ihit=0; ihit<vHit->size(); ihit++){
					THitStorage *hit = &(vHit->at(ihit));
					if(common::AreSame(vZcoord->at(iz), hit->z, EPSILON)){
						if(isWithinCuts(hit)){
							h2xy->Fill(hit->x, hit->y);
						}
					}
				}
				vh2.push_back(h2xy);
			}
		}
	}
}

void TEventAnalysis::RandomizeHitPos(){
	TRandom3 r;
	for(uint16_t ihit=0; ihit<vHit->size(); ihit++){
		THitStorage *hit = &(vHit->at(ihit));
		if(isWithinCuts(hit)){
			THitStorage hitRnd;
			Int_t xRnd = round(r.Uniform(-0.5*PIXEL_SIZE, 0.5*PIXEL_SIZE) + hit->x);
			Int_t yRnd = round(r.Uniform(-0.5*PIXEL_SIZE, 0.5*PIXEL_SIZE) + hit->y);
			hitRnd.Fill(hit->scModule, hit->scPixel, xRnd, yRnd, hit->z, hit->t);
			vHitRnd.push_back(hitRnd);
		}
	}
}

void TEventAnalysis::FillHitPosGraph(vector<TGraph2D*> &vgr){
	FillHitPosGraphWith(vgr, 0, 2, vHit);
	if(vHitRnd.size()>0){
		FillHitPosGraphWith(vgr, 2, 3, &vHitRnd);
	}
}

void TEventAnalysis::DeleteHitsWithBadTiming(double tthr){
	// Find pairs, with the time difference > tthr and remove the point further away from average for this event
	bool restart = true;
	while(restart){
		restart = false;
		uint16_t vPointsSize = vHit->size();

		// Calculate average time (but count time of multiple pixels in a single SC16 only once)
		double avg=0;
		vector<int16_t> vUniqueScModule;
		for(uint16_t ih=0; ih<vPointsSize; ih++){
			THitStorage* hs = &(vHit->at(ih));
			bool scModulePresent = false;
			for(uint16_t isc=0; isc<vUniqueScModule.size(); isc++){
				if(vUniqueScModule.at(isc) == hs->scModule){
					scModulePresent = true;
				}
			}
			if(!scModulePresent){
				vUniqueScModule.push_back(hs->scModule);
				avg += hs->t;
			}
		}
		avg /= vUniqueScModule.size();

		for(uint16_t ih1=0; ih1<vPointsSize; ih1++){
			THitStorage* hs1 = &(vHit->at(ih1));
			for(uint16_t ih2=ih1+1; ih2<vPointsSize; ih2++){
				THitStorage* hs2 = &(vHit->at(ih2));
				int dt = abs(hs1->t - hs2->t);
				if(dt > tthr){
					double davgp1 = fabs(avg - hs1->t);
					double davgp2 = fabs(avg - hs2->t);
					if(davgp1 > davgp2){
						vHit->erase(vHit->begin()+ih1);
					}
					else{
						vHit->erase(vHit->begin()+ih2);
					}
					restart=true;
					break;
				}
			}
			if(restart) break;
		}
	}
}

bool TEventAnalysis::isPatternPresent(THitStorage* hit){
	return hit->scPixel != -1;
}

bool TEventAnalysis::isTimePresent(THitStorage* hit){
	return hit->t != 0;
}

bool TEventAnalysis::isTimeWithinPromptPeak(THitStorage* hit){
	return (hit->t >= cuts->promptT0) && (hit->t <= cuts->promptT1);
}

bool TEventAnalysis::isWithinCuts(THitStorage* hit){
	if(cuts->ignoreHitsWithoutPattern && !isPatternPresent(hit)){
		return false;
	}
	if(cuts->ignoreHitsWithoutTiming && !isTimePresent(hit)){
		return false;
	}
	if(cuts->acceptHitsFromPromptPeakOnly && !isTimeWithinPromptPeak(hit)){
		return false;
	}
	return true;
}

void TEventAnalysis::FillHitPosGraphWith(vector<TGraph2D*> &vgr, UInt_t index0, UInt_t index1, vector<THitStorage>* vHitIn){
	if(index1 > vgr.size()){
		index1 = vgr.size();
	}
	vector<int> vn(index1-index0, 0);
	// Check how many points to allocate in each graph
	for(uint16_t ihit=0; ihit<vHitIn->size(); ihit++){
		THitStorage *hit = &(vHitIn->at(ihit));
		if(isWithinCuts(hit)){
			vn.at(0)++;
		}
		else{
			if(index1-index0 > 1){ // Fill to the second graph the hits, that did not pass the cuts
				vn.at(1)++;
			}
		}
	}

	// Allocate the number of points to graphs
	for(UInt_t igr=index0; igr<index1; igr++){
		vgr.at(igr)->Set(vn.at(igr-index0));
	}

	std::fill(vn.begin(), vn.end(), 0);
	// Fill the graphs (could be optimized - in the first step save indices of hits for each group; then save the correct hits directly to graphs here)
	for(uint16_t ihit=0; ihit<vHitIn->size(); ihit++){
		THitStorage *hit = &(vHitIn->at(ihit));
		if(isWithinCuts(hit)){
			vgr.at(index0)->SetPoint(vn.at(0)++, hit->x, hit->y, hit->z);
		}
		else{
			if(index1-index0 > 1){ // Fill to the second graph the hits, that did not pass the cuts
				vgr.at(index0+1)->SetPoint(vn.at(1)++, hit->x, hit->y, hit->z);
			}
		}
	}
}

// Getters / setters
int16_t TEventAnalysis::getLevelsPresent() const {
	return nLevelsPresent;
}

const vector<int16_t>& TEventAnalysis::getLevelPixelMultiplicity() const {
	return vLevelPixelMultiplicity;
}

const vector<double>& TEventAnalysis::getAvgX() const {
	return vAvgX;
}

const vector<double>& TEventAnalysis::getAvgY() const {
	return vAvgY;
}

const vector<double>& TEventAnalysis::getZcoord() const{
	return *vZcoord;
}

const vector<int16_t>& TEventAnalysis::getLevelSc16Multiplicity() const {
	return vLevelSc16Multiplicity;
}

const vector<double>& std::TEventAnalysis::getSigma2X() const {
	return vSigma2X;
}

const vector<double>& std::TEventAnalysis::getSigma2Y() const {
	return vSigma2Y;
}

const TCuts* TEventAnalysis::getCuts(){
	return cuts;
}

const vector<THitStorage>* TEventAnalysis::getHit(){
	return vHit;
}

const TFileStorage* TEventAnalysis::getEvent(){
	return event;
}

int64_t TEventAnalysis::getEventNumberWithinRun() const {
	return event->iEventNumberWithinRun;
}

const int TEventAnalysis::getRunNumber() const {
	return event->iRunNumber;
}

void TEventAnalysis::PrintHits(){
	for(UInt_t i=0; i<vHit->size(); i++){
		cout << "   " << vHit->at(i).scModule << "|";
		cout << vHit->at(i).scPixel << "  \t(";
		cout << vHit->at(i).x << ", ";
		cout << vHit->at(i).y << ", ";
		cout << vHit->at(i).z << ", ";
		cout << vHit->at(i).t << ")" << endl;
	}
}

} /* namespace std */
