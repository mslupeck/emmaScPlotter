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
#include "common.h"

namespace std {

class EmmaEventTreeReader {
private:
	static constexpr UInt_t N_PIXELS = 16;
	static constexpr UInt_t PIXEL_SIZE = 123;
	static constexpr UInt_t PIXEL_GAP = 1;
	string filePath;
	string fileBaseName;
	TFile *f;
	TTree *t;					// main tree read from the root file
	TFileStorage *fs;			// pointer to the object holding file data

	void ListPlaneCoords(vector<int16_t>& vout, UInt_t evnLimit=100);

public:
	EmmaEventTreeReader();
	virtual ~EmmaEventTreeReader();

	int OpenRootFile(const string &filePath);
	int ReadTree(const string &treeName);
	int ReadFileStorage(const string &objectName);

	void PrintStorageContents(ostream &out, uint64_t evn);

	void AnalysePatternTimingCorrelation();
	void AnalyseMultiplicityPerLevel();

};
} /* namespace std */

#endif /* EMMAEVENTTREEREADER_H_ */
