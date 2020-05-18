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
#include "TReconstruct.h"

using namespace std;

void RunBasic(EmmaEventTreeReader &eetr){
	TApplication theApp("App",0,nullptr);{
		if(eetr.ReadTreeFromRootFile() < 0) return;

		// Data analysis methods
//			for(int i=0; i<1e4; i++){
//				eetr.PrintStorageContents(cout, i);
//			}
		eetr.AnalysePatternTimingCorrelation();
		eetr.AnalyseEventTimeDiffSpectrum();
		eetr.AnalyseRawScTimeSpectrum();
	}
	theApp.SetIdleTimer(10000,"exit()"); // exits the program after 10000 s of inactivity
	theApp.SetReturnFromRun(true);       //
	cout << "Running done." << endl;
	theApp.Run();
}

void RunRawMulti(EmmaEventTreeReader &eetr){
	TApplication theApp("App",0,nullptr);{
		if(eetr.ReadTreeFromRootFile() < 0) return;
		eetr.AnalyseTimingBetweenLevels();
		eetr.AnalyseMultiplicityPerLevel();
		eetr.AnalyseMultiplicityCorrelationBetweenLevels();
	}
	theApp.SetIdleTimer(10000,"exit()"); // exits the program after 10000 s of inactivity
	theApp.SetReturnFromRun(true);       //
	cout << "Running done." << endl;
	theApp.Run();
}

void RunReconstruct(EmmaEventTreeReader &eetr){
	TApplication theApp("App",0,nullptr);{
		if(eetr.ReadTreeFromRootFile() < 0) return;
		TReconstruct r(&eetr, 10);
		r.RunReconstruct();
	}
	theApp.SetIdleTimer(10000,"exit()"); // exits the program after 10000 s of inactivity
	theApp.SetReturnFromRun(true);       //
	cout << "Running done." << endl;
	theApp.Run();
}

void RunVisualize(EmmaEventTreeReader &eetr){
	TApplication theApp("App",0,nullptr);{
		if(eetr.ReadTreeFromRootFile() < 0) return;
		TVisualize vis;
		TEventAnalysis ea(&(eetr.GetFileStorage()->at(0)->vHitPoint), eetr.GetCuts()->n0, eetr.getFileBaseName(), eetr.GetCuts(), eetr.getZcoord());
		ea.AnalyseLevelMultiplicity();
		vis.VisualizeSingleEvent(&ea, eetr.getScMap());
	}
	theApp.SetIdleTimer(10000,"exit()"); // exits the program after 10000 s of inactivity
	theApp.SetReturnFromRun(true);       //
	cout << "Running done." << endl;
	theApp.Run();
}

int main(int argc, char* argv[]){
	// Initialize command-line parser
	cliParser cli;
	cli.AddPar("m", "Mode of operation", true);
	cli.GetModeList()->AddPar("basic", "Reads original root file and plots basic test histos");
	cli.GetModeList()->AddPar("rawMulti", "Reads original root file and plots raw multiplicity histos");
	cli.GetModeList()->AddPar("reco", "Do event reconstruction");
	cli.GetModeList()->AddPar("vis", "Do event visualization");
	cli.AddPar("i", "Input path", true);
	cli.AddPar("o", "Output name", true);
	cli.AddPar("n0", "Start from this event number", false);
	cli.AddPar("nMax", "Max number of events to be processed", false);
	cli.AddPar("promptT0", "Prompt peak cut - start time [default = -2e9]", false);
	cli.AddPar("promptT1", "Prompt peak cut - end time [default = 2e9]", false);
	cli.AddPar("ignoreHitsWithoutTiming", "true / false [default: false]", false);
	cli.AddPar("ignoreHitsWithoutPattern", "true / false [default: false]", false);
	if(cli.Parse(argc, argv)!=0){ // Parse command-line input parameters
		cli.PrintUsage(cout, "emmaScPlotter");
		return -1;
	}

	string inputPath = cli.GetParString("i");
	string outputName = cli.GetParString("o");
	EmmaEventTreeReader eetr;
	const string treeName = "tr", objectName = "event";
	eetr.SetInputFileInfo(inputPath, treeName, objectName);

	// The following can be used in case of trouble to exclude some files (very poor performance)
	//vector<int> viFileBad;
	//viFileBad.push_back(64);
	//eetr.FilterOutBadFiles(viFileBad);

	// Setup cuts to be used by many "eetr.Analyse..()" methods
	bool ignoreHitsWithoutTiming = false, ignoreHitsWithoutPattern = false;
	if(cli.IsParDefined("ignoreHitsWithoutTiming")) ignoreHitsWithoutTiming = cli.GetParBool("ignoreHitsWithoutTiming");
	if(cli.IsParDefined("ignoreHitsWithoutPattern")) ignoreHitsWithoutPattern = cli.GetParBool("ignoreHitsWithoutPattern");
	eetr.setIgnoreHitsWithoutTiming(ignoreHitsWithoutTiming);
	eetr.setIgnoreHitsWithoutPattern(ignoreHitsWithoutPattern);
	int t0 = -2e9, t1 = 2e9;
	if(cli.IsParDefined("promptT0")) t0 = cli.GetParInt("promptT0");
	if(cli.IsParDefined("promptT1")) t1 = cli.GetParInt("promptT1");
	eetr.setAcceptHitsFromPromptPeakOnly(true, t0, t1);
	int nMax = -1, n0 = -1;
	if(cli.IsParDefined("nMax")) nMax = cli.GetParInt("nMax");
	if(cli.IsParDefined("n0")) n0 = cli.GetParInt("n0");
	eetr.setEventNumberCuts(n0, nMax);
	cout << "  <I> The following settings are used: " << endl;
	cout << "  <I>   promptT0 = " << t0 << endl;
	cout << "  <I>   promptT1 = " << t1 << endl;
	cout << "  <I>   ignoreHitsWithoutTiming = " << std::boolalpha << ignoreHitsWithoutTiming << endl;
	cout << "  <I>   ignoreHitsWithoutPattern = " << std::boolalpha << ignoreHitsWithoutPattern << endl;
	cout << "  <I>   n0 = " << n0 << endl;
	cout << "  <I>   nMax = " << nMax << endl;

	if(cli.GetParString("m").compare("basic")==0){
		RunBasic(eetr);
	}
	else if(cli.GetParString("m").compare("rawMulti")==0){
		RunRawMulti(eetr);
	}
	else if(cli.GetParString("m").compare("reco")==0){
		RunReconstruct(eetr);
	}
	else if(cli.GetParString("m").compare("vis")==0){
		RunVisualize(eetr);
	}
	else{
		cout << "Unknown mode: " << cli.GetParString("m") << endl;
		cli.PrintUsage(cout,"emmaScPlotter");
	}
	cout << "Done." << endl;

	return 0;
}
