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

#include "TFileStorage.h"
#include "TEventAnalysis.h"
#include "common.h"

namespace std {

class EmmaEventTreeReader {
private:
	string filePath;
	string fileBaseName;
	TFile *f;
	TTree *t;                     // pointer to currently used tree (one of the above)
	TFileStorage *fs;			  // currently used fs, pointing to either of the tree entries

	vector<int16_t> vZcoord;
	void ListPlaneCoords(vector<int16_t>& vout, Int_t evnLimit=100);
	void SetupPad(TH1* h, float fontSize, float lmargin, float rmargin, float tmargin, float bmargin, float xoffset=1, float yoffset=1, float zoffset=1);
	void DrawTextNdc(std::string s, double x, double y, double size, Color_t col=kBlack, Float_t tangle=0);

public:
	EmmaEventTreeReader();
	virtual ~EmmaEventTreeReader();

	int OpenRootFile(const string &filePath);
	int ReadTree(const string &treeName);
	int ReadFileStorage(const string &objectName);

	void PrintStorageContents(ostream &out, int64_t evn);

	void AnalysePatternTimingCorrelation();
	void AnalyseEventTimeSpectrum();
	void AnalyseRawScTimeSpectrum();
	void AnalyseMultiplicityPerLevel();
};
} /* namespace std */

#endif /* EMMAEVENTTREEREADER_H_ */
