/*
 * TEventAnalysis.h
 *
 *  Created on: Dec 3, 2019
 *      Author: mss
 */

#ifndef TEVENTANALYSIS_H_
#define TEVENTANALYSIS_H_

#include <iostream>
#include <sstream>
#include <vector>
#include <TH1D.h>
#include <TH2D.h>
#include <TMath.h>

#include "TFileStorage.h"
#include "TCuts.h"

namespace std {

class TEventAnalysis {
private:
	static constexpr UInt_t N_PIXELS = 16;
	static constexpr UInt_t PIXEL_SIZE = 123;
	static constexpr UInt_t PIXEL_GAP = 1;
	static constexpr float  DXY_PIXEL = 0.5 * (PIXEL_SIZE + PIXEL_GAP);
	static constexpr float  EPSILON = 0.01;

	vector<THitStorage> *vHit;
	TCuts *cuts;

	vector<int16_t> *vZcoord;
	vector<int16_t> vLevelMultiplicity;
	int16_t nLevelsPresent;

	bool isPatternPresent(THitStorage* hit);
	bool isTimePresent(THitStorage* hit);
	bool isTimeWithinPromptPeak(THitStorage* hit);

public:
	TEventAnalysis(vector<THitStorage> *vHit, TCuts *cuts);
	virtual ~TEventAnalysis();

	void AnalyseLevelMultiplicity(vector<int16_t> *vZcoord);
	void FillLayerHistos(vector<TH2D*> &vh2);
	void FillHbTbHistos(vector<int> &vTimeCnt, vector<int> &vHbCnt, vector<int> &vTimeAndHbCnt, vector<int> &vTimeOrHbCnt);
	void FillRawScTimeHistos(vector<TH2D*> &vh2);

	// Setters / getters
	int16_t getLevelsPresent() const;
	const vector<int16_t>& getLevelMultiplicity() const;
};

} /* namespace std */

#endif /* TEVENTANALYSIS_H_ */
