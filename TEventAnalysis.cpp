/*
 * TEventAnalysis.cpp
 *
 *  Created on: Dec 3, 2019
 *      Author: mss
 */

#include "TEventAnalysis.h"

namespace std {

TEventAnalysis::TEventAnalysis(vector<THitStorage> *vHit, TCuts *cuts) {
	this->vHit = vHit;
	this->vZcoord = nullptr;
	nLevelsPresent = 0;
	this->cuts = cuts;
}

TEventAnalysis::~TEventAnalysis() {
	// TODO Auto-generated destructor stub
}

void TEventAnalysis::AnalyseLevelMultiplicity(vector<int16_t> *vZcoord){
	this->vZcoord = vZcoord;
	// Check number of hits per layer
	vector<int> vLevelMultiplicity(vZcoord->size(), 0);
	for(UInt_t ihit=0; ihit<vHit->size(); ihit++){
		THitStorage* hit = &(vHit->at(ihit));
		if(cuts->ignoreHitsWithoutPattern && !isPatternPresent(hit)){
			continue;
		}
		if(cuts->ignoreHitsWithoutTiming && !isTimePresent(hit)){
			continue;
		}
		if(cuts->acceptHitsFromPromptPeakOnly && !isTimeWithinPromptPeak(hit)){
			continue;
		}
		for(UInt_t iz=0; iz<vZcoord->size(); iz++){
			if(vZcoord->at(iz) == hit->z){
				vLevelMultiplicity.at(iz)++;
			}
		}
	}
	for(UInt_t iz=0; iz<vLevelMultiplicity.size(); iz++){
		if(vLevelMultiplicity.at(iz) > 0){
			nLevelsPresent++;
		}
	}
}

void TEventAnalysis::FillLayerHistos(vector<TH2D*> &vh2){
	if(vZcoord == nullptr){
		cout << " <W> TEventAnalysis::FillLayerHistos(): uninitialized vZcoord; run AnalyseLevelMultiplicity() beforehand." << endl;
	}
	for(UInt_t ihit=0; ihit<vHit->size(); ihit++){
		THitStorage* hit = &(vHit->at(ihit));
		if(cuts->ignoreHitsWithoutPattern && !isPatternPresent(hit)){
			continue;
		}
		if(cuts->ignoreHitsWithoutTiming && !isTimePresent(hit)){
			continue;
		}
		if(cuts->acceptHitsFromPromptPeakOnly && !isTimeWithinPromptPeak(hit)){
			continue;
		}
		for(UInt_t iz=0; iz<vZcoord->size(); iz++){
			if(vZcoord->at(iz) == hit->z){
				if(hit->scPixel == -1){ // if no pattern info present: paint all 16 pixels with 1/16th weight
					for(UInt_t ipixel=0; ipixel<N_PIXELS; ipixel++){
						float dx = DXY_PIXEL*(2. * (ipixel%4) - 3);
						float dy = DXY_PIXEL*(2. * (ipixel/4) - 3);
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

void TEventAnalysis::FillRawScTimeHistos(vector<TH2D*> &vh2){
	double avgT = 0;
	UInt_t nT = 0;
	for(UInt_t ihit=0; ihit<vHit->size(); ihit++){
		THitStorage* hit = &(vHit->at(ihit));
		if(cuts->ignoreHitsWithoutPattern && !isPatternPresent(hit)){
			continue;
		}
		if(cuts->ignoreHitsWithoutTiming && !isTimePresent(hit)){
			continue;
		}
		if(cuts->acceptHitsFromPromptPeakOnly && !isTimeWithinPromptPeak(hit)){
			continue;
		}
		if(hit->t > EPSILON){
			vh2.at(0)->Fill(hit->t, nLevelsPresent);
			avgT += hit->t;
			nT++;
		}
	}
	avgT /= nT;
	double rmsT = 0;
	for(UInt_t ihit=0; ihit<vHit->size(); ihit++){
		THitStorage* hit = &(vHit->at(ihit));
		if(cuts->ignoreHitsWithoutPattern && !isPatternPresent(hit)){
			continue;
		}
		if(cuts->ignoreHitsWithoutTiming && !isTimePresent(hit)){
			continue;
		}
		if(cuts->acceptHitsFromPromptPeakOnly && !isTimeWithinPromptPeak(hit)){
			continue;
		}
		if(hit->t > EPSILON){
			rmsT += TMath::Power(hit->t-avgT, 2);
		}
	}
	rmsT /= nT;
	rmsT = TMath::Sqrt(rmsT);
	vh2.at(1)->Fill(avgT, nLevelsPresent);
	vh2.at(2)->Fill(rmsT, nLevelsPresent);
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

// Getters / setters
int16_t TEventAnalysis::getLevelsPresent() const {
	return nLevelsPresent;
}

const vector<int16_t>& TEventAnalysis::getLevelMultiplicity() const {
	return vLevelMultiplicity;
}

} /* namespace std */
