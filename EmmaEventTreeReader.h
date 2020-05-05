/*
 * EmmaEventTreeReader.h
 *
 *  Created on: Nov 18, 2019
 *      Author: mss
 */

#ifndef EMMAEVENTTREEREADER_H_
#define EMMAEVENTTREEREADER_H_

#include <iostream>
#include <iomanip>
#include <string>
#include <time.h>

#include <TDirectory.h>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TCanvas.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TAxis.h>
#include <TStyle.h>
#include <TRandom3.h>

#include "TFileStorage.h"
#include "TCuts.h"
#include "common.h"

namespace std {

class EmmaEventTreeReader {
private:
	string filePath;
	string treeName;
	string objectName;
	string fileBaseName;
	vector<TFileStorage*> vfs;     // copy of the file contents from the tree to memory
	TCuts cuts;

	vector<int16_t> vZcoord;
	void ListPlaneCoords(vector<int16_t>& vout, Int_t evnLimit=100);

	vector<Color_t> vCol; // set of colors for histos

public:
	EmmaEventTreeReader();
	virtual ~EmmaEventTreeReader();

	int ReadTreeFromRootFile();

	void PrintStorageContents(ostream &out, int64_t evn);

	void AnalysePatternTimingCorrelation();
	void AnalyseEventTimeDiffSpectrum();
	void AnalyseRawScTimeSpectrum();
	void AnalyseMultiplicityPerLevel();
	void AnalyseMultiplicityCorrelationBetweenLevels();
	void AnalyseTimingBetweenLevels();

	// Cuts
	void FilterOutBadFiles(vector<int> &viFile);

	// Getter / setter
	void setIgnoreHitsWithoutPattern(bool ignoreHitsWithoutPattern);
	void setIgnoreHitsWithoutTiming(bool ignoreHitsWithoutTiming);
	void setAcceptHitsFromPromptPeakOnly(bool acceptHitsFromPromptPeakOnly, int t0, int t1);

	void SetInputFileInfo(const string &filePath, const string &treeName, const string &objectName);
};
} /* namespace std */

#endif /* EMMAEVENTTREEREADER_H_ */
