/*
 * main.cpp
 *
 *  Created on: Nov 18, 2019
 *      Author: mss
 */

#include <iostream>
//#include <algorithm>

#include <TSystem.h>
#include <TApplication.h>

#include "common.h"
#include "cliParser.h"
#include "EmmaEventTreeReader.h"

using namespace std;

int main(int argc, char* argv[]){
	cliParser cli;
	cli.AddPar("m", "Mode of operation", true);
	cli.GetModeList()->AddPar("read", "Reads original root file and plots basic test histos");
	cli.AddPar("i", "Input path", true);
	cli.AddPar("o", "Output name", true);
	if(cli.Parse(argc, argv)!=0){
		cli.PrintUsage(cout, "emmaScPlotter");
		return -1;
	}

	string inputPath = cli.GetParString("i");
	string outputName = cli.GetParString("o");
	if(cli.GetParString("m").compare("read")==0){
		TApplication theApp("App", &argc, argv);{

			EmmaEventTreeReader eetr;
			if(eetr.OpenRootFile(inputPath) < 0) return -1;
			string treeName = "tr";
			if(eetr.ReadTree(treeName) < 0) return -2;
			string objectName = "event";
			if(eetr.ReadFileStorage(objectName) < 0) return -3;
//			for(int i=0; i<1e5; i++){
//				eetr.PrintStorageContents(cout, i);
//			}
//			eetr.AnalysePatternTimingCorrelation();
//			eetr.AnalyseEventTimeSpectrum();
			eetr.AnalyseRawScTimeSpectrum();
//			eetr.AnalyseMultiplicityPerLevel();

		}
		theApp.SetIdleTimer(100000,"exit()");
		theApp.SetReturnFromRun(true);
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
