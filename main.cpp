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
		int nEntries = eetr.GetFileStorage()->size();
		// If additional cut on the event time is set then find the corresponding event, otherwise use evn = 0
		int evn = 0;
		double t0 = 0;
		bool draw = false;
		if((eetr.GetCuts()->t0 + 1) > 0.001){
			for(evn = 0; evn<nEntries; evn++){
				TFileStorage* fs = eetr.GetFileStorage()->at(evn);
				t0 = fs->fEventTimeS + 1.e-9*fs->lFileStartTimeNs + fs->lFileStartTimeS;
				if((t0 + 0.000002) > eetr.GetCuts()->t0){ // increase by 2 us to find the event indicated by this time, not the subsequent event
					cout << "  <I> main:RunVisualize(): Found evn:" << evn << " with time:" << t0 << " following input:" << fixed << eetr.GetCuts()->t0 << endl;
					draw = true;
					break;
				}
			}
		}
		else{
			draw = true;
		}
		if(draw){
			TVisualize vis;
			TEventAnalysis ea(eetr.GetFileStorage()->at(evn), eetr.GetCuts(), eetr.getZcoord());
			ea.AnalyseLevelMultiplicity();
			vis.VisualizeSingleEvent(&ea, eetr.getScMap());
		}
		else{
			cout << "<E> main:RunVisualize(). Time > " << fixed << eetr.GetCuts()->t0 << " not found. Try a further run." << endl;
		}
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
	cli.AddPar("t0", "Start from event starting after this time [1577224064.607941]; only in -m vis", false);
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
	int promptt0 = -2e9, promptt1 = 2e9;
	if(cli.IsParDefined("promptT0")) promptt0 = cli.GetParInt("promptT0");
	if(cli.IsParDefined("promptT1")) promptt1 = cli.GetParInt("promptT1");
	eetr.setAcceptHitsFromPromptPeakOnly(true, promptt0, promptt1);
	int nMax = -1, n0 = -1;
	double t0 = -1;
	if(cli.IsParDefined("nMax")) nMax = cli.GetParInt("nMax");
	if(cli.IsParDefined("n0")) n0 = cli.GetParInt("n0");
	if(cli.IsParDefined("t0")) t0 = cli.GetParDouble("t0");
	eetr.setEventNumberCuts(n0, t0, nMax);
	cout << "  <I> The following settings are used: " << endl;
	cout << "  <I>   promptT0 = " << promptt0 << endl;
	cout << "  <I>   promptT1 = " << promptt1 << endl;
	cout << "  <I>   ignoreHitsWithoutTiming = " << std::boolalpha << ignoreHitsWithoutTiming << endl;
	cout << "  <I>   ignoreHitsWithoutPattern = " << std::boolalpha << ignoreHitsWithoutPattern << endl;
	cout << "  <I>   n0 = " << n0 << endl;
	cout << "  <I>   nMax = " << nMax << endl;
	cout << "  <I>   t0 = " << fixed << t0 << endl;

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
