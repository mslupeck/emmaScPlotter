/*
 * common.h
 *
 *  Created on: Mar 6, 2017
 *      Author: mss
 */

typedef short int int16_t;
typedef short unsigned int uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;

#ifndef COMMON_H_
#define COMMON_H_

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

#include <TF1.h>
#include <TF2.h>
#include <TH1.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TH3F.h>
#include <TRandom3.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TSystemDirectory.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TFrame.h>
#include <TAxis.h>
#include <TPaletteAxis.h>
#include <TPaveStats.h>
#include <TColor.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TLatex.h>
#include <TMath.h>

class common {
public:
	common();
	virtual ~common();

	static Color_t GetCol(int i);
	static bool AreSame(double x, double y, double epsilon=0.0001);
	static bool IsBetween(double x, double x0, double x1);
	static void ListFiles(std::vector<std::string> &result, const char *dirname, const char *ext=".root", int16_t verbose=0);
	static int GetFileSize(const std::string &path);
	static void Message(char type, std::string source, std::string message, int16_t verbosity = 0, std::ostream& out = std::cout);
	static int GetNDaysPerMonth(int month, int year); // month: 0-11, year: 1900-2200
	static int ExtractRunNumber(std::string *filename, const std::string runnumberprefix="RMM");
	static int ExtractFileNumber(std::string *filename, const std::string filenumberprefix="-F");
	static Int_t FindBin(TH2D* h2, double x, double y);
	static Int_t FindBin(TH2D* h2, int x, int y);
	static Int_t FindBin(TH2F* h2, int x, int y);
	static Int_t FindBin(TH3D* h3, double x, double y, double z);
	static TGraphErrors* OffsetX(const TGraphErrors* gr, int offset=0, int axisoffset=0, bool mirror=false);
	static TH1D* OffsetX(const TH1D* h2, int offset=0, int axisoffset=0, bool mirror=false);
	static TH2D* OffsetX(const TH2D* h2, int offset=0, int axisoffset=0, bool mirror=false);
	static TH1D* NormPerSrZenith(TH1D* h1);
	static TH1D* NormPerSrAzimuth(TH1D* h1);
	static TH2D* NormPerSr(TH2D* h2);
	static void SetWorldDirAxisLabels(TAxis* ax);
	static void DrawWorldDirAxisLabels(double x1, double x2, double y, double ymin, double ymax, double labelfontsize=0.1, Color_t col=kBlack);
	static TH2D* Interpolate(TH2D* h2);
	static TH3D* Interpolate(TH3D* h3);
	static double GetXFromTH2(TH2* h2, const double y, const double z, const double epsilon=1e-5); // works only for monotonic-rising functions
	static int ProduceAsciiFromTH2D(TH2D* h2, ostream& out);
	static void Multiply(TH2D* h, double f);

	static void SetupRootStyle();
	static void SetupPad(TH1* h, float fontSize=0.07, float lmargin=0.1, float rmargin=0.1, float tmargin=0.1, float bmargin=0.1, float xoffset=1.0, float yoffset=1.0, float zoffset=1.0);
	static void DrawAndAdjust(TH1* h, std::string option, float x1ndc, float y1ndc, float x2ndc, float y2ndc, float linewidth=1.5, Color_t col=kBlack);
	static void DrawWithTwoAxes(unsigned int ipad, TH1* h1, TH1* h2, Float_t xmin, Float_t xmax, Float_t ymin1, Float_t ymax1, Float_t ymin2, Float_t ymax2, float legx1=0.45, float legy1=0.76, float legx2=0.99, float legy2=0.89, int linewidth1=1.5, int linewidth2=3, bool logScale=true);
	static void DrawWithTwoAxes2(unsigned int ipad, TH1* h1, TH1* h2, Float_t xmin, Float_t xmax, Float_t ymin1, Float_t ymax1, Float_t ymin2, Float_t ymax2, float legx1=0.45, float legy1=0.76, float legx2=0.99, float legy2=0.89, int linewidth1=1.5, int linewidth2=3, bool logScale=true);
	static void DrawWithTwoAxesMulti(unsigned int ipad, std::vector<TH1*> vh1, std::vector<TH1*> vh2, Float_t xmin, Float_t xmax, Float_t ymin1, Float_t ymax1, Float_t ymin2, Float_t ymax2, bool logScale=true);
	static void SetBinContentAndBinomialError(TH1* h, int32_t ibin, float nTrg, float N, float multiplFactor=1);
	static void ArrangeStatBox(TH1* h1, int i, float x0, float y0, float w, float h); // i-number of plot at the same pad
	static float AxisCoordsToNDC(TAxis *a, float x);

	static void DrawTextNdc(std::string s, double x, double y, double size, Color_t col=kBlack, Float_t tangle=0);
	static void DrawTextUser(std::string s, double x, double y, double size, Color_t col=kBlack, Float_t tangle=0);
	static void Sqrt(TH2D* h2);
	static void FillUntouchedBins(TH2D* h2, double value);


	static double CalculateSolidAngleTest();
	// Based on MC, returns solid angle in [sr] limited by 2 rectangles at the distance d
	static double CalculateSolidAngle(double x, double y, double z);
	// Based on MC, returns solid angle in [sr] limited by n rectangles (x0,y0,dx,dy,z0), starting with randomizing hit position on the first level
	static double CalculateSolidAngle(double *x0, double *y0, double *z0, double *dx, double *dy, const int n);
	// Based on MC, returns solid angle in [sr] limited by n rectangles (x0,y0,dx,dy,z0), starting with muons randomized on the half-sphere surrounding detector
	static double CalculateSolidAngleFromSphere(double *x0, double *y0, double *z0, double *dx, double *dy, const int n);

};

#endif /* COMMON_H_ */
