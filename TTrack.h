/*
 * TTrack.h
 *
 *  Created on: May 5, 2020
 *      Author: mss
 */

#include <TGraph2D.h>
#include <vector>

#ifndef TTRACK_H_
#define TTRACK_H_

class TTrack {
private:
	static const int nPars = 4;
	double parFit[nPars];
	double pStart[nPars];
	double amin;
	double globcc;
	float phi;
	float theta;

	static double Distance2(double x,double y,double z, double *p);
	static void SumDistance2(int &, double *, double & sum, double * par, int );

public:
	TTrack();
	void CalculateFitInitialParams(const std::vector<double> &vx, const std::vector<double> &vy, const std::vector<double> &vz);
	void Line3Dfit(TGraph2D *gr);

	double getAmin() const;
	double getGlobcc() const;
	double* getParFit();
	void setParStart(double parStart[4]);
	float getPhi() const;
	float getTheta() const;
};

#endif /* TTRACK_H_ */
