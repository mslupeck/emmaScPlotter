/*
 * TTrack.cpp
 *
 *  Created on: May 5, 2020
 *      Author: mss
 */

#include "TTrack.h"

#include <TVirtualFitter.h>
#include <Math/Vector3D.h>
#include <assert.h>
#include <TVector3.h>
#include <TMath.h>

TTrack::TTrack() {
	for(int ipar=0; ipar<nPars; ipar++){
		parFit[ipar] = 0;
		pStart[ipar] = 0;
	}
	amin = 0;
	globcc = 0;
	phi = 0;
	theta = 0;
}

// calculate distance line-point
double TTrack::Distance2(double x,double y,double z, double *p) {
	// distance line point is D= | (xp-x0) cross  ux |
	// where ux is direction of line and x0 is a point in the line (like t = 0)
	ROOT::Math::XYZVector xp(x,y,z);
	ROOT::Math::XYZVector x0(p[0], p[2], 0. );
	ROOT::Math::XYZVector x1(p[0] + p[1], p[2] + p[3], 1. );
	ROOT::Math::XYZVector u = (x1-x0).Unit();
	double d2 = ((xp-x0).Cross(u)) .Mag2();
	return d2;
}

// function to be minimized
void TTrack::SumDistance2(int &, double *, double & sum, double * par, int ) {
	// the TGraph must be a global variable
	TGraph2D * gr = dynamic_cast<TGraph2D*>( (TVirtualFitter::GetFitter())->GetObjectFit() );
	assert(gr != 0);
	double * x = gr->GetX();
	double * y = gr->GetY();
	double * z = gr->GetZ();
	int npoints = gr->GetN();
	sum = 0;
	for (int i  = 0; i < npoints; ++i) {
		double d = Distance2(x[i],y[i],z[i],par); // d should be increased when the weight is larger
		sum += d;
	}
}

//void TTrack::CalculateFitInitialParams(double xBot, double yBot, double zBot, double xTop, double yTop, double zTop){
void TTrack::CalculateFitInitialParams(const std::vector<double> &vx, const std::vector<double> &vy, const std::vector<double> &vz){
	// Calculate the average hit position in 2 bottom levels and in top 2 levels
	const float eps = 0.01;
	double xBot = 0, yBot = 0, zBot = 0, xTop = 0, yTop = 0, zTop = 0;
	UInt_t ntop = 0, nbot = 0;
	UInt_t nLevels = vx.size();
	for(UInt_t ilevel = 0; ilevel < 2; ilevel++){
		if(vx.at(ilevel) > eps){ // if x != 0 then y != 0 automatically
			xBot += vx.at(ilevel);
			yBot += vy.at(ilevel);
			zBot += vz.at(ilevel);
			nbot++;
		}
	}
	for(UInt_t ilevel = nLevels-2; ilevel < nLevels; ilevel++){
		if(vx.at(ilevel) > eps){ // if x != 0 then y != 0 automatically
			xTop += vx.at(ilevel);
			yTop += vy.at(ilevel);
			zTop += vz.at(ilevel);
			ntop++;
		}
	}
	xBot /= nbot;
	yBot /= nbot;
	zBot /= nbot;
	xTop /= ntop;
	yTop /= ntop;
	zTop /= ntop;

	double dx = xTop-xBot;
	double dy = yTop-yBot;
	double dz = zTop-zBot;
	pStart[1] = dx/dz;
	pStart[3] = dy/dz;
	pStart[0] = (xBot*zTop-xTop*zBot)/(zTop-zBot);
	pStart[2] = (yBot*zTop-yTop*zBot)/(zTop-zBot);
	// std::cout << "  " << dx << " " << dy << " " << dz << std::endl;
	// std::cout << "  " << pStart[0] << " " << pStart[1] << " " << pStart[2] << " " << pStart[3] << std::endl;
}

void TTrack::Line3Dfit(TGraph2D *gr)
{
	// fit the graph now
	TVirtualFitter *min = TVirtualFitter::Fitter(0,4);
	min->SetObjectFit(gr);
	min->SetFCN(TTrack::SumDistance2);

	Double_t arglist1[1];
	arglist1[0] = -1; // print nothing, but warnings
	//arglist1[0] = 0; // print much
	min->ExecuteCommand("SET PRINT", arglist1, 1);

	Double_t arglist2[3]={0,0,0};
	min->ExecuteCommand("SET NOWarnings", arglist2, 3);

	min->SetParameter(0, "x0", pStart[0], 0.01, 0.0, 0.0);
	min->SetParameter(1, "Ax", pStart[1], 0.01, 0.0, 0.0);
	min->SetParameter(2, "y0", pStart[2], 0.01, 0.0, 0.0);
	min->SetParameter(3, "Ay", pStart[3], 0.01, 0.0, 0.0);

	Double_t arglist[2];
	arglist[0] = 1000;  // number of function calls
	arglist[1] = 0.001; // tolerance
	min->ExecuteCommand("MIGRAD",arglist,2);

	int nvpar=0,nparx=0;
	double edm=0, errdef=0;
	min->GetStats(amin,edm,errdef,nvpar,nparx);

	int ipar=0;
	double eplus=0, eminus=0, eparab=0;
	min->GetErrors(ipar, eplus, eminus, eparab, globcc);

	// get fit parameters
	for(int i=0; i<4; ++i){
		parFit[i] = min->GetParameter(i);
	}
	min->Delete();

	TVector3 *v3 = new TVector3(parFit[1], parFit[3], 1);
	theta = 180.*v3->Theta()/TMath::Pi();
	phi = 180.*v3->Phi()/TMath::Pi();
}

double TTrack::getAmin() const {
	return amin;
}

double TTrack::getGlobcc() const {
	return globcc;
}

double* TTrack::getParFit(){
	return parFit;
}

void TTrack::setParStart(double parStart[4]){
	for(int ipar=0; ipar<nPars; ipar++){
		pStart[ipar] = parStart[ipar];
	}
}

float TTrack::getPhi() const {
	return phi;
}

float TTrack::getTheta() const {
	return theta;
}
