/*
 * main.cpp
 *
 *  Created on: Nov 18, 2019
 *      Author: mss
 */

// C++ includes
#include <iostream>

// Root-related includes
#include <TSystem.h>
#include <TApplication.h>

// Self-written classes
#include "common.h"
#include "cliParser.h"
#include "EmmaEventTreeReader.h"

using namespace std;

int main(int argc, char* argv[]){
	// Initialize command-line parser
	cliParser cli;
	cli.AddPar("m", "Mode of operation", true);
	cli.GetModeList()->AddPar("read", "Reads original root file and plots basic test histos");
	cli.AddPar("i", "Input path", true);
	cli.AddPar("o", "Output name", true);
	if(cli.Parse(argc, argv)!=0){ // Parse command-line input parameters
		cli.PrintUsage(cout, "emmaScPlotter");
		return -1;
	}

	// In general cliParser allows for several modes of operation, for now only one is used
	string inputPath = cli.GetParString("i");
	string outputName = cli.GetParString("o");
	if(cli.GetParString("m").compare("read")==0){
		// This is needed to be able to see any canvases
		TApplication theApp("App", &argc, argv);{

			// Main root file reader class, that includes also analysing & plotting methods
			EmmaEventTreeReader eetr;
			string treeName = "tr", objectName = "event";
			if(eetr.ReadTreeFromRootFile(inputPath, treeName, objectName) < 0) return -1;

			// The following can be used in case of trouble to exclude some files (very poor performance)
			//vector<int> viFileBad;
			//viFileBad.push_back(64);
			//eetr.FilterOutBadFiles(viFileBad);

			// Setup cuts to be used by many "Analyse..()" methods
			eetr.setIgnoreHitsWithoutTiming(true);
			eetr.setIgnoreHitsWithoutPattern(true);
			eetr.setAcceptHitsFromPromptPeakOnly(true, 507200, 508400);

			// Data analysis methods
//			for(int i=0; i<1e4; i++){
//				eetr.PrintStorageContents(cout, i);
//			}
//			eetr.AnalysePatternTimingCorrelation();
			eetr.AnalyseEventTimeSpectrum();
//			eetr.AnalyseRawScTimeSpectrum();
//			eetr.AnalyseMultiplicityPerLevel();

		}
		theApp.SetIdleTimer(10000,"exit()"); // exits the program after 10000 s of inactivity
		theApp.SetReturnFromRun(true);       //
		cout << "Running done." << endl;
		theApp.Run();
	}
	else{
		cout << "Unknown mode: " << cli.GetParString("m") << endl;
		cli.PrintUsage(cout,"emmaScPlotter");
	}
	cout << "Done." << endl;

	return 0;
}
