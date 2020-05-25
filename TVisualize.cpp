/*
 * TVisualize.cpp
 *
 *  Created on: May 5, 2020
 *      Author: mss
 */

#include "TVisualize.h"
#include <TPolyLine3D.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TView3D.h>
#include <TViewer3DPad.h>

#include "common.h"

namespace std {

TVisualize::TVisualize() {
	scMap = nullptr;
}

TVisualize::~TVisualize() {
	// TODO Auto-generated destructor stub
}

void TVisualize::SetViewport(double x0, double y0, double z0, double x1, double y1, double z1, float fontsize, float xoffset, float yoffset, float zoffset, char option){
	const float halfSize = 0.55;
	const float extraz = 0.2;
	float dx = halfSize*(x1-x0);
	float dy = halfSize*(y1-y0);
	float dz = halfSize*(z1-z0);
	float dxy = max(dx, dy);
	float x = 0.5*(x0+x1);
	float y = 0.5*(y0+y1);
	float z = 0.5*(z0+z1);

	TView3D* view = (TView3D*) TView::CreateView(1);
	view->SetRange(	x - dxy, y - dxy, z - dz - extraz*dz,
					x + dxy, y + dxy, z + dz);
	switch(option){
	case 'f':
		view->Front();
		break;
	case 's':
		view->Side();
		break;
	case 't':
		view->Top();
		break;
	default:
		break;
	}

	TGraph2D *dummy = new TGraph2D(2);
	dummy->SetTitle("");
	dummy->GetXaxis()->SetTitle("x [mm]");
	dummy->GetYaxis()->SetTitle("y [mm]");
	dummy->GetZaxis()->SetTitle("z [mm]");
	dummy->GetXaxis()->SetTitleOffset(xoffset);
	dummy->GetYaxis()->SetTitleOffset(yoffset);
	dummy->GetZaxis()->SetTitleOffset(zoffset);
	dummy->GetXaxis()->SetLabelSize(fontsize);
	dummy->GetXaxis()->SetTitleSize(fontsize);
	dummy->GetYaxis()->SetLabelSize(fontsize);
	dummy->GetYaxis()->SetTitleSize(fontsize);
	dummy->GetZaxis()->SetLabelSize(fontsize);
	dummy->GetZaxis()->SetTitleSize(fontsize);

	stringstream ss;
	ss << "dummy" << "";
	dummy->SetName(ss.str().c_str());
	dummy->SetPoint(0, x - dxy, y - dxy, z - dz - extraz*dz);
	dummy->SetPoint(1, x + dxy, y + dxy, z + dz);
	dummy->SetLineWidth(0);
	dummy->SetMarkerSize(0);
	dummy->Draw("p");
}

void TVisualize::DrawBoxWireframe(double xc, double yc, double zc, double wx, double wy, double wz, uint16_t linewidth, Color_t linecol){
	float x0 = xc - wx/2;
	float y0 = yc - wy/2;
	float z0 = zc - wz/2;

	const uint16_t nlines = 12;
	TPolyLine3D *l[nlines];
	for(uint16_t iline=0; iline<nlines; iline++){
		l[iline] = new TPolyLine3D(2);
		l[iline]->SetLineColor(linecol);
		l[iline]->SetLineWidth(linewidth);
	}
	// bottom side
	l[0]->SetPoint(0,x0   ,y0   ,z0   );
	l[0]->SetPoint(1,x0+wx,y0   ,z0   );
	l[1]->SetPoint(0,x0+wx,y0   ,z0   );
	l[1]->SetPoint(1,x0+wx,y0+wy,z0   );
	l[2]->SetPoint(0,x0+wx,y0+wy,z0   );
	l[2]->SetPoint(1,x0   ,y0+wy,z0   );
	l[3]->SetPoint(0,x0   ,y0+wy,z0   );
	l[3]->SetPoint(1,x0   ,y0   ,z0   );
	// top side
	l[4]->SetPoint(0,x0   ,y0   ,z0+wz);
	l[4]->SetPoint(1,x0+wx,y0   ,z0+wz);
	l[5]->SetPoint(0,x0+wx,y0   ,z0+wz);
	l[5]->SetPoint(1,x0+wx,y0+wy,z0+wz);
	l[6]->SetPoint(0,x0+wx,y0+wy,z0+wz);
	l[6]->SetPoint(1,x0   ,y0+wy,z0+wz);
	l[7]->SetPoint(0,x0   ,y0+wy,z0+wz);
	l[7]->SetPoint(1,x0   ,y0   ,z0+wz);
    // verticals
	l[8] ->SetPoint(0,x0   ,y0   ,z0   );
	l[8] ->SetPoint(1,x0   ,y0   ,z0+wz);
	l[9] ->SetPoint(0,x0+wx,y0   ,z0   );
	l[9] ->SetPoint(1,x0+wx,y0   ,z0+wz);
	l[10]->SetPoint(0,x0+wx,y0+wy,z0   );
	l[10]->SetPoint(1,x0+wx,y0+wy,z0+wz);
	l[11]->SetPoint(0,x0   ,y0+wy,z0   );
	l[11]->SetPoint(1,x0   ,y0+wy,z0+wz);

	for(uint16_t iline=0; iline<nlines; iline++){
		l[iline]->Draw("same");
	}
}

void TVisualize::DrawScPixel(double xc, double yc, double zc, uint16_t linewidth, Color_t linecol){
	DrawBoxWireframe(xc, yc, zc, dxy, dxy, dz, linewidth, linecol);
}

void TVisualize::DrawScModuleWithoutPixels(double xc, double yc, double zc, uint16_t linewidth, Color_t linecol){
	DrawBoxWireframe(xc, yc, zc, scdxy, scdxy, dz, linewidth, linecol);
}

void TVisualize::DrawScModule(double x, double y, double z, uint16_t linewidth, Color_t linecol){
	float xc = x + 0.5*dxy;
	float yc = y + 0.5*dxy;
	float zc = z + 0.5*dz;
	for(UInt_t ix=0; ix<N_PIXELSX; ix++){
		for(UInt_t iy=0; iy<N_PIXELSY; iy++){
			DrawScPixel(xc+dxy*ix, yc+dxy*iy, zc, linewidth, linecol);
		}
	}
}

void TVisualize::DrawPb(uint16_t linewidth, Color_t linecol){
	DrawBoxWireframe(1000, 750, 738, 1000, 1000, 50, linewidth, linecol);
}

void TVisualize::DrawNeutronTubes(uint16_t linewidth, Color_t linecol){
	// Positions of neutron center
	float xc = 1000;
	float yc = 750;
	float zc = 796;
	float wx = 550; // length of tubes
	float dy = 50; // distance between tubes
	const uint16_t nTubes = 15;
	float wy = dy*(nTubes-1);
	for(uint16_t iline=0; iline<nTubes; iline++){
		if(iline == 14) continue; // last empty place for tube
		TPolyLine3D *l = new TPolyLine3D(2);
		l->SetLineColor(linecol);
		l->SetLineWidth(linewidth);
		l->SetPoint(0, xc - wx/2, yc - wy/2 + iline*dy, zc);
		l->SetPoint(1, xc + wx/2, yc - wy/2 + iline*dy, zc);
		l->Draw("same");
	}


}

void TVisualize::DrawScSetup(uint16_t linewidth, Color_t linecol){
	for(UInt_t isc=0; isc<scMap->GetScMap()->size(); isc++){
		TSingleScMap* s = &(scMap->GetScMap()->at(isc));
		DrawScModule(s->xCoord, s->yCoord, s->zCoord + DS_SC16Z_CASE, linewidth, linecol);
	}
}

void TVisualize::Line(double t, double *p, double &x, double &y, double &z) {
	// a parameteric line is define from 6 parameters but 4 are independent
	// x0,y0,z0,z1,y1,z1 which are the coordinates of two points on the line
	// can choose z0 = 0 if line not parallel to x-y plane and z1 = 1;
	x = p[0] + p[1]*t;
	y = p[2] + p[3]*t;
	z = t;
}

void TVisualize::DrawFired(TGraph2D *gr, Color_t col){
	if((gr != nullptr) && (gr->GetN() > 0)){
		for(uint16_t ipoint=0; ipoint<gr->GetN(); ipoint++){
			double x,y,z;
			x = gr->GetX()[ipoint];
			y = gr->GetY()[ipoint];
			z = gr->GetZ()[ipoint];

			// Rough and non-universal check if the pattern was missing
			if(abs((int)round(x-0.5*dxy)%dxy) > dxy/4){
				DrawScModuleWithoutPixels(x, y, z, 1.5, col);
			}
			else{
				DrawScPixel(x, y, z, 1.5, col);
			}
		}
		gr->SetMarkerColor(col);
		gr->SetFillColor(col);
		gr->SetMarkerStyle(8);
		gr->Draw("same p");
	}
}

void TVisualize::DrawFit(double* parFit, uint16_t linewidth, Color_t linecol){
	// draw the fitted line
	TPolyLine3D *l = new TPolyLine3D(2);
	double t0 = -100;
	double dt = 3200;
	for (int i = 0; i<l->GetN(); ++i) {
		double t = t0+ dt*i/l->GetN();
		double x,y,z;
		Line(t,parFit,x,y,z);
		l->SetPoint(i,x,y,z);
	}
	l->SetLineColor(linecol);
	l->SetLineWidth(linewidth);
	l->Draw("same");
}

void TVisualize::SaveForVis(TEventAnalysis* ea, uint16_t maxNVis){
	static uint16_t evVis = 0;
	if(evVis<maxNVis){ // visualize selected events
		vector<TGraph2D*> vgr;
		vgr.push_back(new TGraph2D());
		vgr.push_back(new TGraph2D());
		ea->FillHitPosGraph(vgr);

		int izLast = ea->getZcoord().size()-1;
		if(izLast < 1){
			cout << " <W> TVisualize::SaveForVis(): Event multiplicity is too low: " << ea->getZcoord().size() << endl;
			return;
		}

		TTrack track;
		track.CalculateFitInitialParams(ea->getAvgX(), ea->getAvgY(), ea->getZcoord());
		track.Line3Dfit(vgr.at(0));
		vTrack.push_back(track);

		vvGrVis.push_back(vgr);
		evVis++;
	}
}

void TVisualize::VisualizeMulti(TScMapReader* scMap){
	this->scMap = scMap;
	if(vvGrVis.size() > 0){
		TCanvas *cvsVis3d = new TCanvas("cvsVis3d", "vis3d", 1700, 850);
		cvsVis3d->Divide(4,3);
		for(UInt_t igr=0; igr<12; igr++){
			cvsVis3d->cd(igr+1);
			DrawScSetup();
			DrawPb(2.5, kBlack);
			DrawNeutronTubes(2, kRed+3);
			if(vvGrVis.size() > igr){
				DrawFired(vvGrVis.at(igr).at(0), kBlue-2);
				DrawFired(vvGrVis.at(igr).at(1), kRed);
				DrawFit(vTrack.at(igr).getParFit(), 2.5, kRed);
			}
		}
	}
	else{
		cout << "<E> TVisualize::VisualizeMulti(): No graphs to visualize. Exiting." << endl;
	}
}

void TVisualize::CreateViewAndDrawAll(TEventAnalysis* ea, int evn, float fontsize, float xoffset, float yoffset, float zoffset, char option){
	float wx = 0.5 * (scMap->GetMaxXCoord() - scMap->GetMinXCoord() + scdxy);
	float wy = 0.5 * (scMap->GetMaxYCoord() - scMap->GetMinYCoord() + scdxy);
	float cx = scMap->GetMinXCoord() + wx;
	float cy = scMap->GetMinYCoord() + wy;
	SetViewport(cx - wx, cy - wy, scMap->GetMinZCoord(),
				cx + wx, cy + wy, scMap->GetMaxZCoord() + dz, fontsize, xoffset, yoffset, zoffset, option);
	DrawScSetup(1, kGray);
	DrawPb(2.5, kBlack);
	DrawNeutronTubes(2, kRed+3);
	if(vvGrVis.at(0).size() >= 1){
		DrawFired(vvGrVis.at(evn).at(0), kBlue-2);
		DrawFired(vvGrVis.at(evn).at(1), kRed);
		DrawFit(vTrack.at(evn).getParFit(), 2.5, kRed);
	}
}

void TVisualize::VisualizeSingleEvent(TEventAnalysis* ea, TScMapReader* scMap){
	const int evn = 0;
	this->scMap = scMap;
	float fontsize = 0.07, lmargin = 0.15, rmargin = 0.03, tmargin = 0.03, bmargin = 0.2;
	SaveForVis(ea, 1);
	const float vsplit = 0.7;
	if(vvGrVis.size() == 1){
		stringstream ssCvsName;
		ssCvsName << "cVis-R" << ea->getRunNumber() << "-" << ea->getEventNumberWithinRun();
		TCanvas *cvsVisSingle = new TCanvas("cVisSingle", ssCvsName.str().c_str(), 1700, 850);
		TPad *pmain = new TPad("pmain", "pmain", 0, 0, vsplit, 1);
		TPad *pright = new TPad("pmain", "pmain", vsplit, 0, 1, 1);

		cvsVisSingle->cd();
		pmain->Draw();
		pmain->cd();
			gPad->SetMargin(0.1, 0.1, 0.13, 0.1);
			CreateViewAndDrawAll(ea, evn, 0.5*fontsize, 1.8, 1.8, 1.4);
			stringstream ss1, ss2;
			if(vTrack.size()>0){
				ss1 << "R" << ea->getRunNumber() << ",    event=" << ea->getEventNumberWithinRun() << ",    ";
				ss1 << "t=" << fixed << ea->getEvent()->fEventTimeS + ea->getEvent()->lFileStartTimeS + 1e-9*ea->getEvent()->lFileStartTimeNs;
				ss2 << "#phi = " << round(vTrack.at(0).getPhi()) << "#circ,  ";
				ss2 << "#theta = " << round(vTrack.at(0).getTheta()) << "#circ";
			}
			common::DrawTextNdc(ss1.str().c_str(), 0.02, 0.98-0.65*fontsize*1, 0.65*fontsize, kBlue-2);
			common::DrawTextNdc(ss2.str().c_str(), 0.02, 0.98-0.65*fontsize*2, 0.65*fontsize, kBlue-2);
		cvsVisSingle->cd();
		pright->Draw();
		pright->Divide(1,3);
		pright->cd(1);
			gPad->SetMargin(lmargin, rmargin, bmargin, tmargin);
			CreateViewAndDrawAll(ea, evn, fontsize, 1.2, 1.2, 1.2, 't');
			common::DrawTextNdc("Top", 0.83, 0.85, fontsize*1.5);
		pright->cd(2);
			gPad->SetMargin(lmargin, rmargin, bmargin, tmargin);
			CreateViewAndDrawAll(ea, evn, fontsize, 0.6, 1.2, 1.2, 'f');
			common::DrawTextNdc("Front", 0.83, 0.85, fontsize*1.5);
		pright->cd(3);
			gPad->SetMargin(lmargin, rmargin, bmargin, tmargin);
			CreateViewAndDrawAll(ea, evn, fontsize, 1.2, 0.6, 1.2, 's');
			common::DrawTextNdc("Side", 0.83, 0.85, fontsize*1.5);
	}
	else{
		cout << "<E> TVisualize::VisualizeSingleEvent(): No graphs to visualize. Exiting." << endl;
	}
}

} /* namespace std */
