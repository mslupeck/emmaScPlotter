/*
 * TReconstruct.cpp
 *
 *  Created on: May 5, 2020
 *      Author: mss
 */

#include <iostream>

#include "TReconstruct.h"
#include "TVisualize.h"
#include "TFileStager.h"
#include "TEventAnalysis.h"

namespace std {

TReconstruct::TReconstruct(EmmaEventTreeReader *eetr, int verbose) {
	this->eetr = eetr;
	this->verbose = verbose;
}

TReconstruct::~TReconstruct() {}

void TReconstruct::RunReconstruct(){
	// Main event loop
	vector<TFileStorage*>* vfs = eetr->GetFileStorage();
	int nEntries = vfs->size();
	for(int evn=0; evn<nEntries; evn++){
//		cout << endl << "=== " << evn << " =============================================" << endl;
		TEventAnalysis eaOrg(&(vfs->at(evn)->vHitPoint), evn, eetr->getFileBaseName(), eetr->GetCuts(), eetr->getZcoord());
		eaOrg.AnalyseLevelMultiplicity();
		if(verbose >= 2){
			if((evn%100000) == 0){
				cout << " EvnProgress: " << evn << "/" << nEntries << " [" << ((float)round(1000.0*evn/nEntries))*0.1 << "%]" << endl;
			}
		}
		visualizer.SaveForVis(&eaOrg);
//		TEventAnalysis eaProcessed(eaOrg);

//		eaProcessed.DeleteHitsWithBadTiming();

//		eaOrg.PrintHits();
//		cout << "=== " << endl;
//		eaProcessed.PrintHits();

		/*
		eOrg->DeleteHitsWithBadTiming();
		eOrg->CalculateMLevel();
		eOrg->CalculateClosestNeighbors(126); // closest: sqrt(2)*halfPixelSize = 89  ||  also diagonals: 2*halfPixelSize = 126

		TGraph2D *gr = new TGraph2D();
		bool grSaved = false;
		double parStart[4], amin=0, globcc=0;
		TEvent *e = new TEvent(eOrg);
		e->Average2Neighbors(); // TODO: change so that it can work together with randomization for any number of neighbors
		e->CalculateMLevel();
		e->CalculateFitInitialParams(parStart);
		e->CalculateProjection(parStart);
		//e->CalculateProjection(parStart, 62.5);
		ts->projectionOk = e->projectionOK;
		if(e->projectionOK){
			evM1orM2aNeighborsProjection++;
			RandomizeHitPostions(e, rnd);
			e->CalculateFitInitialParams(parStart);

			FillGraph(gr,e);
			double parFit[4];
			Line3Dfit(gr, parFit, parStart, amin, globcc);

			TVector3 *v3 = new TVector3(parFit[1], parFit[3], 1);
			double theta = 180.*v3->Theta()/TMath::Pi();
			double phi = 180.*v3->Phi()/TMath::Pi();
			v3->Delete();
			double d = sqrt(amin);

			double dphi = min(abs(realphi-phi), abs(360-abs(realphi-phi)));
			double dtheta = abs(realtheta-theta);
		}
		*/
	}

	visualizer.VisualizeMulti(eetr->getScMap());

/*	bool doVisualize = true;
	uint16_t evn=12;
	cout << "NEvents to visualize: " << evn << "..+12 / " << vGrVis.size() << endl;
	if(doVisualize){
		evn+=12;
		if(vGrVis.size()<evn){
			cout << "Too few events to visualize: " << vGrVis.size() << endl;
			return;
		}
		TCanvas *cvsVis3d = new TCanvas("Vis3d","Vis3d",1800,1000);
		SetViewportSize(cvsVis3d, -500,-500,-50, 2000,2000,450);
		for(int ipad=0; ipad<12; ipad++){
			cvsVis3d->cd(ipad+1);
			DrawScSetup(-250,-250,-15,-125,-125,15);
			Draw3dFit(vGrVis.at(evn-12+ipad), &(vParFitVis.at((evn-12+ipad)*4)));
		}
	}
*/
}



} /* namespace std */
