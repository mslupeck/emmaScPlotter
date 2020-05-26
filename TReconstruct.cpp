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
	vector<TH2D*> vh2control;
	vh2control.push_back(new TH2D("h2spreadVsMulti","h2spreadVsMulti", 3000, 0, 3000, 200, 0, 200));
	gStyle->SetOptStat("iou");
	vector<TH2D*> vh2;

	// Main event loop
	vector<TFileStorage*>* vfs = eetr->GetFileStorage();
	int nEntries = vfs->size();
	for(int evn=0; evn<nEntries; evn++){
		TEventAnalysis eaOrg(vfs->at(evn), eetr->GetCuts(), eetr->getZcoord());
//		cout << endl << "=== R" << eaOrg.getRunNumber() << ":" << eaOrg.getEventNumberWithinRun() << " =============================================" << endl;
		if(verbose >= 2){
			if((evn%100000) == 0){
				cout << " EvnProgress: " << evn << "/" << nEntries << " [";
				cout.precision(1);
				cout << ((float)round(1000.0*evn/nEntries))*0.1 << "%]" << endl;
			}
		}
		eaOrg.AnalyseLevelMultiplicity();
		TEventAnalysis eaProcessed(eaOrg);
		eaProcessed.DeleteHitsWithBadTiming();
		eaProcessed.AnalyseLevelMultiplicity(); // again because now some hits have been removed
		eaProcessed.RandomizeHitPos();

		// Clusterizing tests
		//eaProcessed.FillHitPosLevel(vh2control, vh2, 1, 3, 1, 160);

		visualizer.SaveForVis(eaProcessed);


		/* // Old MM code
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

/*	// Clusterizing tests
	TCanvas *cvsSpread = new TCanvas("cvsSpread","cvsSpread",1800,600);
	cvsSpread->Divide(3,1);
	cvsSpread->cd(1);
		gPad->SetLogy();
		vh2control.at(0)->ProjectionX()->Draw();
	cvsSpread->cd(2);
		gPad->SetLogy();
		vh2control.at(0)->ProjectionY()->Draw();
	cvsSpread->cd(3);
		gPad->SetLogy();
		vh2control.at(0)->Draw("colz");

	TCanvas *cvsXy = new TCanvas("cvsXy","cvsXy",1750,850);
	cvsXy->Divide(12,6);
	for(UInt_t i=0; i<72; i++){
		cvsXy->cd(i+1);
		gPad->SetMargin(0, 0, 0, 0.2);
		if(i < vh2.size()){
			vh2.at(i)->Draw("colz");
			common::DrawTextNdc(vh2.at(i)->GetName(), 0.0, 0.8, 0.2);
		}
	}
*/
	visualizer.VisualizeMulti(eetr->getScMap());
}



} /* namespace std */
