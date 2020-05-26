/*
 * TVisualize.h
 *
 *  Created on: May 5, 2020
 *      Author: mss
 */

#include <vector>
#include <string>
#include <sstream>
#include <TGraph2D.h>

#include "TScMapReader.h"
#include "TEventAnalysis.h"
#include "TTrack.h"
#include "TView3D.h"

#ifndef TVISUALIZE_H_
#define TVISUALIZE_H_

namespace std {

struct TVisStorage{
	TEventAnalysis *ea;
	vector<TGraph2D*> vGrVis;
	vector<TTrack> vTrack;
	TVisStorage(TEventAnalysis &ea) {this->ea = new TEventAnalysis(ea);};
};

class TVisualize {
private:
	vector<TVisStorage*> vVisStorage;
	TScMapReader* scMap;

	bool isEvnStoredForVis(UInt_t evn);
	void DrawBoxWireframe(double xc, double yc, double zc, double wx, double wy, double wz, uint16_t linewidth = 1, Color_t linecol = kGray);
	void SetViewport(double x0, double y0, double z0, double x1, double y1, double z1, float fontsize, float xoffset, float yoffset, float zoffset, char option='\0');
	void DrawScPixel(double x0, double y0, double z0, uint16_t linewidth = 1, Color_t linecol = kGray);
	void DrawScModuleWithoutPixels(double xc, double yc, double zc, uint16_t linewidth = 1, Color_t linecol = kGray);
	void DrawScModule(double x, double y, double z, uint16_t linewidth = 1, Color_t linecol = kGray);
	void DrawScSetup(uint16_t linewidth = 1, Color_t linecol = kGray);
	void DrawPb(uint16_t linewidth = 1, Color_t linecol = kGray);
	void DrawNeutronTubes(uint16_t linewidth, Color_t linecol);
	void CreateViewAndDrawAll(UInt_t evn, float fontsize, float xoffset, float yoffset, float zoffset, char option='\0');
	void Line(double t, double *p, double &x, double &y, double &z);
	void DrawFired(TGraph2D *gr, Color_t col);
	void DrawFit(double* parFit, uint16_t linewidth = 1, Color_t linecol = kGray);

	// Pixel sizes
	const int32_t dxy = DS_SC1_GAP + DS_SC1;
	const float dz = 30;

	// SC16 size
	const float scdxy = dxy * N_PIXELSX;

public:
	TVisualize();
	virtual ~TVisualize();

	void SaveForVis(TEventAnalysis &eaIn, uint16_t maxNVis = 100);
	void VisualizeMulti(TScMapReader* scMap);
	void VisualizeSingleEvent(TEventAnalysis &eaIn, TScMapReader* scMap);
};

} /* namespace std */

#endif /* TVISUALIZE_H_ */
