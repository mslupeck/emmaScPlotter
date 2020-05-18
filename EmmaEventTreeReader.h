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

#include "TScMapReader.h"
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
	TScMapReader scMap;
	TCuts cuts;

	vector<double> vZcoord;
	void ListPlaneCoords(vector<double>& vout, Int_t evnLimit=1000);

	vector<Color_t> vCol; // set of colors for histos

public:
	EmmaEventTreeReader();
	virtual ~EmmaEventTreeReader();

	int ReadTreeFromRootFile(bool initAllUtilities = true);

	void PrintStorageContents(ostream &out, int64_t evn);

	// Pre-analysis checks
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
	void setEventNumberCuts(int n0, int nMax);

	void SetInputFileInfo(const string &filePath, const string &treeName, const string &objectName);
	vector<TFileStorage*>* GetFileStorage();
	vector<double>* getZcoord();
	TCuts* GetCuts();
	TScMapReader* getScMap();
	string& getFileBaseName();
};
} /* namespace std */

#endif /* EMMAEVENTTREEREADER_H_ */
