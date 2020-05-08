/*
 * TEventAnalysis.h
 *
 *  Created on: Dec 3, 2019
 *      Author: mss
 *  Class holding methods and data fields related to data processing at the event level
 */

#ifndef TEVENTANALYSIS_H_
#define TEVENTANALYSIS_H_

#include <iostream>
#include <sstream>
#include <vector>
#include <TH1D.h>
#include <TH2D.h>
#include <TGraph2D.h>
#include <TMath.h>

#include "TFileStorage.h"
#include "TCuts.h"

namespace std {

class TEventAnalysis {
private:
	static constexpr UInt_t N_PIXELS_PER_SCM = 16;
	static constexpr UInt_t PIXEL_SIZE = 123;
	static constexpr UInt_t PIXEL_GAP = 1;
	static constexpr float  DXY_PIXEL = 0.5 * (PIXEL_SIZE + PIXEL_GAP);
	static constexpr float  EPSILON = 0.01;

	vector<THitStorage> *vHit; // pointer to the vector of TTree entries
	TCuts *cuts;               // class storing cut settings

	// z-coordinates of the system (if 4 detector layers are present in the file
	// then this vector has 4 entries listing the z-position of each layer)
	vector<double> *vZcoord;

	// Hit multiplicity of each layer (only available after running AnalyseLevelMultiplicity(..))
	vector<int16_t> vLevelPixelMultiplicity;
	vector<int16_t> vLevelSc16Multiplicity;
	vector<double> vAvgX;		// average x-pos for each level
	vector<double> vAvgY;		// average y-pos for each level
	vector<double> vSigma2X;	// standard deviation in x-direction for each level
	vector<double> vSigma2Y;	// standard deviation in y-direction for each level

	// Number of layers that have at least one pixel hit
	int16_t nLevelsPresent;

	bool isPatternPresent(THitStorage* hit);
	bool isTimePresent(THitStorage* hit);
	bool isTimeWithinPromptPeak(THitStorage* hit);
	bool isWithinCuts(THitStorage* hit);

public:
	TEventAnalysis(vector<THitStorage> *vHit, TCuts *cuts, vector<double> *vZcoord = nullptr);
	virtual ~TEventAnalysis();

	void AnalyseLevelMultiplicity();
	void AnalyseLevelSc16Multiplicity();
	void FillLayerHistos(vector<TH2D*> &vh2);
	void FillHbTbHistos(vector<int> &vTimeCnt, vector<int> &vHbCnt, vector<int> &vTimeAndHbCnt, vector<int> &vTimeOrHbCnt);
	void FillRawScTimeHistos(TH2D* vh2);
	double FillAvgHitTime(TH1D* h, int16_t zCoord);

	// Tracking / visualization
	void FillHitPosGraph(vector<TGraph2D*> &vgr);

	// Setters / getters
	int16_t getLevelsPresent() const;
	const vector<int16_t>& getLevelPixelMultiplicity() const;
	const vector<int16_t>& getLevelSc16Multiplicity() const;
	const vector<double>& getAvgX() const;
	const vector<double>& getAvgY() const;
	const vector<double>& getZcoord() const;
};

} /* namespace std */

#endif /* TEVENTANALYSIS_H_ */
