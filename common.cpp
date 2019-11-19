/*
 * common.cpp
 *
 *  Created on: Mar 6, 2017
 *      Author: mss
 */

//#include "includes/common.h"
#include "common.h"

common::common() {
	// TODO Auto-generated constructor stub
	}

common::~common() {
	// TODO Auto-generated destructor stub
}

Color_t common::GetCol(int i){
	switch(i){
	case 0:
		return kBlack; break;
	case 1:
		return kRed; break;
	case 2:
		return kPink-9; break;
	case 3:
		return kMagenta; break;
	case 4:
		return kViolet+1; break;
	case 5:
		return kBlue; break;
	case 6:
		return kAzure+1; break;
	case 7:
		return kCyan+1; break;
	case 8:
		return kTeal-8; break;
	case 9:
		return kGreen+1; break;
	case 10:
		return kGreen+1; break;
	case 11:
		return kSpring+3; break;
	case 12:
		return kYellow+1; break;
	case 13:
		return kOrange+1; break;
	default:
		return kGray; break;
	}
}

bool common::AreSame(double x, double y, double epsilon){
	if(fabs(x-y)<epsilon){
		return true;
	}
	return false;
}

bool common::IsBetween(double x, double x0, double x1){
	if(x0 > x1){ // adjust range, such that x0 is always smaller than x1
		double tmpx = x0;
		x0 = x1;
		x1 = tmpx;
	}
	return ((x > x0) && (x < x1));
}

void common::ListFiles(std::vector<std::string> &result, const char *dirname, const char *ext, int16_t verbose)
{
	if(verbose>0){
		std::cout << "  <I> common::ListFiles():Reading directory: " << dirname << std::endl;
	}

	TSystemDirectory dir(dirname, dirname);
	TList *files = dir.GetListOfFiles();
	if (files) {
		TSystemFile *file;
		TString fname;
		TIter next(files);
		while ((file=(TSystemFile*)next())) {
			fname = file->GetName();
			if (!file->IsDirectory() && fname.EndsWith(ext)) {
				std::string filename = fname.Data();
//				if(filename.find("@")==std::string::npos){
					result.push_back(filename);
//				}
			}
		}
	}
	sort(result.begin(),result.end());
	if(verbose>1){
		for(uint16_t ifile=0; ifile<result.size(); ifile++){
			std::cout << "      - ListFiles(): " << ifile << " - " << result.at(ifile) << std::endl;
		}
	}
	if(verbose>=0){
		std::cout << "  <I> common::ListFiles():Directory read."<< std::endl;
	}
}

int common::GetFileSize(const std::string &path){
	ifstream f(path.c_str(), std::ios::binary | std::ios::ate);
	return f.tellg();
}

void common::Message(char type, std::string source, std::string message, int16_t verbosity, std::ostream& out){
	std::stringstream ss;
	bool doPrint = false;
	if(type == 'i'){ // info
		if(verbosity > 2){
			doPrint = true;
		}
		ss << "   <I> ";
	}
	else if(type == 'w'){ // warning
		if(verbosity > 1){
			doPrint = true;
		}
		ss << "  <W> ";
	}
	else if(type == 'e'){ // error
		if(verbosity > 0){
			doPrint = true;
		}
		ss << " <E> ";
	}
	else if(type == 'f'){ // fatal
		if(verbosity >= 0){
			doPrint = true;
		}
		ss << "<F> ";
	}

	if(doPrint){
		ss << source << ": " << message << std::endl;
		out << ss.str();
	}
}

int common::GetNDaysPerMonth(int month, int year){
	switch(month){
	case 0:
		return 31;
	case 1: // February
		if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)){
			return 29;
		}
		else {
			return 28;
		}
	case 2:
		return 31;
	case 3:
		return 30;
	case 4:
		return 31;
	case 5:
		return 30;
	case 6:
		return 31;
	case 7:
		return 31;
	case 8:
		return 30;
	case 9:
		return 31;
	case 10:
		return 30;
	case 11:
		return 31;
	default:
		std::cout << " <W> common::GetNDaysPerMonth():Wrong month number provided [0-11]: " << month << std::endl;
		return -1;
	}
}

int common::ExtractRunNumber(std::string *filename, const std::string runnumberprefix){
	size_t pos = filename->find(runnumberprefix);
	std::stringstream ss(&(filename->at(pos+runnumberprefix.size())));
	int runn = 0;
	ss >> runn;
	return runn;
}

int common::ExtractFileNumber(std::string *filename, const std::string filenumberprefix){
	size_t pos = filename->find(filenumberprefix);
	std::stringstream ss(&(filename->at(pos+filenumberprefix.size())));
	int filen = 0;
	ss >> filen;
	return filen;
}

Int_t common::FindBin(TH2D* h2, double x, double y){
	Int_t bin = h2->GetBin(h2->GetXaxis()->FindBin(x), h2->GetYaxis()->FindBin(y), 0);
	return bin;
}

Int_t common::FindBin(TH2D* h2, int x, int y){
	Int_t bin = h2->GetBin(h2->GetXaxis()->FindBin(x), h2->GetYaxis()->FindBin(y), 0);
	return bin;
}

Int_t common::FindBin(TH2F* h2, int x, int y){
	Int_t bin = h2->GetBin(h2->GetXaxis()->FindBin(x), h2->GetYaxis()->FindBin(y), 0);
	return bin;
}

Int_t common::FindBin(TH3D* h3, double x, double y, double z){
	Int_t bin = h3->GetBin(h3->GetXaxis()->FindBin(x), h3->GetYaxis()->FindBin(y), h3->GetZaxis()->FindBin(z));
	return bin;
}

TH1D* common::OffsetX(const TH1D* h, int offset, int axisoffset, bool mirror){
	offset += axisoffset;
	std::stringstream ss;
	ss << h->GetName() << "_+offset" << offset << "-" << axisoffset;
	uint32_t nBinsX = h->GetNbinsX();
	TH1D *hoffset = new TH1D(ss.str().c_str(), ss.str().c_str(), nBinsX, h->GetBinLowEdge(1)+axisoffset, h->GetBinLowEdge(nBinsX+1)+axisoffset);
	hoffset->SetXTitle(h->GetXaxis()->GetTitle());
	hoffset->SetYTitle(h->GetYaxis()->GetTitle());
	for(uint32_t ibin=1; ibin<=nBinsX; ibin++){
		uint32_t ibinmirrored=ibin;
		if(mirror){
			ibinmirrored = nBinsX-ibin+1;
		}
		uint32_t ibinoffset=(ibinmirrored-1+offset)%nBinsX+1;
		hoffset->SetBinContent(ibinoffset, h->GetBinContent(ibin));
		hoffset->SetBinError(ibinoffset, h->GetBinError(ibin));
	}
	return hoffset;
}

TGraphErrors* common::OffsetX(const TGraphErrors* gr, int offset, int axisoffset, bool mirror){
	offset += axisoffset;
	std::stringstream ss;
	ss << gr->GetName() << "_+offset" << offset << "-" << axisoffset;
	uint32_t nPoints = gr->GetN();
	TGraphErrors *groffset = new TGraphErrors(nPoints);
	groffset->SetName(ss.str().c_str());
	groffset->GetXaxis()->SetTitle(gr->GetXaxis()->GetTitle());
	groffset->GetYaxis()->SetTitle(gr->GetYaxis()->GetTitle());
	for(uint32_t ipoint=0; ipoint<=nPoints; ipoint++){
		uint32_t ipointmirrored = ipoint;
		if(mirror){
			ipointmirrored = nPoints-ipoint;
		}
		double x=0, y=0;
		gr->GetPoint(ipointmirrored, x, y);
		double dx = gr->GetErrorX(ipointmirrored);
		double dy = gr->GetErrorY(ipointmirrored);

		int32_t xoffset = static_cast<int>(x+offset);
		if(xoffset<0){
			xoffset += 360;
		}
		if(xoffset>360){
			xoffset -= 360;
		}
		groffset->SetPoint(ipoint, xoffset, y);
		groffset->SetPointError(ipoint, dx, dy);
	}
	return groffset;
}

TH2D* common::OffsetX(const TH2D* h, int offset, int axisoffset, bool mirror){
	offset += axisoffset;
	std::stringstream ss;
	ss << h->GetName() << "_+offset" << offset << "-" << axisoffset;

	TAxis *ax = h->GetXaxis();
	TAxis *ay = h->GetYaxis();
	uint32_t nBinsX = ax->GetNbins();
	uint32_t nBinsY = ay->GetNbins();
	TH2D *hoffset = new TH2D(ss.str().c_str(), ss.str().c_str(),
			                 nBinsX, ax->GetXmin()+axisoffset, ax->GetXmax()+axisoffset,
							 nBinsY, ay->GetXmin(), ay->GetXmax());
	hoffset->SetXTitle(h->GetXaxis()->GetTitle());
	hoffset->SetYTitle(h->GetYaxis()->GetTitle());
	hoffset->SetZTitle(h->GetZaxis()->GetTitle());
	for(uint32_t ibiny=1; ibiny<=nBinsY; ibiny++){
		for(uint32_t ibinx=1; ibinx<=nBinsX; ibinx++){
			uint32_t ibinxmirrored=ibinx;
			if(mirror){
				ibinxmirrored = nBinsX-ibinx+1;
			}
			uint32_t ibin = h->GetBin(ibinx, ibiny);
			uint32_t ibinoffset = h->GetBin((ibinxmirrored-1+offset)%nBinsX+1, ibiny);
			hoffset->SetBinContent(ibinoffset, h->GetBinContent(ibin));
			hoffset->SetBinError(ibinoffset, h->GetBinError(ibin));
		}
	}
	return hoffset;
}

TH1D* common::NormPerSrAzimuth(TH1D* h){
	const double pi = TMath::Pi();

	std::stringstream ss;
	ss << h->GetName() << "_PerSr";
	h->SetName(ss.str().c_str());

	const double scaleAzimuth = 1./h->GetXaxis()->GetNbins();
	const double scalePerSr = 2.*pi;
	double scale = scaleAzimuth*scalePerSr;
	h->Scale(1./scale);

	return h;
}

TH1D* common::NormPerSrZenith(TH1D* h){
	const double pi = TMath::Pi();
	TF1* f = new TF1("f","sin(x)",0,pi);

	std::stringstream ss;
	ss << h->GetName() << "_PerSr";
	h->SetName(ss.str().c_str());
	TAxis *ax = h->GetXaxis();
	uint32_t nBinsX = ax->GetNbins();
	const double scalePerSr = 2.*pi;
	for(uint32_t ibinx=1; ibinx<=nBinsX; ibinx++){
		double scaleZenith = f->Integral(pi*ax->GetBinLowEdge(ibinx)/180, pi*ax->GetBinUpEdge(ibinx)/180);
		double scale = scaleZenith*scalePerSr;
		h->SetBinContent(ibinx, h->GetBinContent(ibinx)/scale);
		h->SetBinError(ibinx, h->GetBinError(ibinx)/scale);
	}
	return h;
}

TH2D* common::NormPerSr(TH2D* h){
	const double pi = TMath::Pi();
	TF1* f = new TF1("f","sin(x)",0,pi);

	std::stringstream ss;
	ss << h->GetName() << "_PerSr";
	h->SetName(ss.str().c_str());
	TAxis *ax = h->GetXaxis();
	TAxis *ay = h->GetYaxis();
	uint32_t nBinsX = ax->GetNbins();
	uint32_t nBinsY = ay->GetNbins();
	const double scaleAzimuth = 1./nBinsX;
	const double scalePerSr = 2.*pi;
	for(uint32_t ibiny=1; ibiny<=nBinsY; ibiny++){
		double scaleZenith = f->Integral(pi*ay->GetBinLowEdge(ibiny)/180, pi*ay->GetBinUpEdge(ibiny)/180);
		double scale = scaleAzimuth*scaleZenith*scalePerSr;
		for(uint32_t ibinx=1; ibinx<=nBinsX; ibinx++){
			uint32_t ibin = h->GetBin(ibinx, ibiny);
			h->SetBinContent(ibin, h->GetBinContent(ibin)/scale);
			h->SetBinError(ibin, h->GetBinError(ibin)/scale);
		}
	}
	return h;
}

void common::SetWorldDirAxisLabels(TAxis* ax){
	std::vector<std::string> vCardinalDirNames;
	vCardinalDirNames.push_back("N");
	vCardinalDirNames.push_back("NNW");
	vCardinalDirNames.push_back("NW");
	vCardinalDirNames.push_back("WNW");
	vCardinalDirNames.push_back("W");
	vCardinalDirNames.push_back("WSW");
	vCardinalDirNames.push_back("SW");
	vCardinalDirNames.push_back("SSW");
	vCardinalDirNames.push_back("S");
	vCardinalDirNames.push_back("SSE");
	vCardinalDirNames.push_back("SE");
	vCardinalDirNames.push_back("ESE");
	vCardinalDirNames.push_back("E");
	vCardinalDirNames.push_back("ENE");
	vCardinalDirNames.push_back("NE");
	vCardinalDirNames.push_back("NNE");
	vCardinalDirNames.push_back("N");

	for(uint16_t istep=0; istep<vCardinalDirNames.size()-1; istep++){
		std::stringstream ss;
		ss << vCardinalDirNames.at(istep);
		ax->SetBinLabel(ax->FindBin(22.5*istep),ss.str().c_str());
		//ax->SetBinLabel(ax->FindBin(istep),ss.str().c_str());
	}
	ax->SetBinLabel(ax->FindBin(359.9), vCardinalDirNames.at(vCardinalDirNames.size()-1).c_str());
	//ax->SetBinLabel(ax->FindBin(15.9), vCardinalDirNames.at(vCardinalDirNames.size()-1).c_str());
}

void common::DrawWorldDirAxisLabels(double x1, double x2, double y, double ymin, double ymax, double labelfontsize, Color_t col){
	std::vector<std::string> vCardinalDirNames;
	vCardinalDirNames.push_back("N");
	vCardinalDirNames.push_back("NE");
	vCardinalDirNames.push_back("E");
	vCardinalDirNames.push_back("SE");
	vCardinalDirNames.push_back("S");
	vCardinalDirNames.push_back("SW");
	vCardinalDirNames.push_back("W");
	vCardinalDirNames.push_back("NW");
	vCardinalDirNames.push_back("N");

	double dstep = (x2-x1)/(vCardinalDirNames.size()-1);
	double linestep = 360./(vCardinalDirNames.size()-1);
	for(uint16_t istep=0; istep<vCardinalDirNames.size(); istep++){
		//std::cout << x1+(istep*dstep) << " " << y << std::endl;
		float nCharactersCorr = (vCardinalDirNames.at(istep).size()-1)*0.25*labelfontsize;
		common::DrawTextNdc(vCardinalDirNames.at(istep), x1+(istep*dstep)-nCharactersCorr, y, labelfontsize, col);

		if((istep!=0) && (istep!=vCardinalDirNames.size()-1)){
			TLine* l0 = new TLine(linestep*istep, ymin+((ymax-ymin)*0.04), linestep*istep, ymax);
			l0->SetLineColor(kWhite);
			l0->SetLineWidth(2);
			l0->SetLineStyle(2);
			l0->Draw();
			TLine* l = new TLine(linestep*istep, ymin+((ymax-ymin)*0.04), linestep*istep, ymax);
			l->SetLineColor(col);
			l->SetLineWidth(1);
			l->SetLineStyle(2);
			l->Draw();
		}
	}
}

TH3D* common::Interpolate(TH3D* h3){
	// Fill vertical gaps with average between closest filled columns
	const double epsilon = 1e-16; // need to provide additional argument to comparison function, since the values read are very small
	std::stringstream ssname;
	ssname << h3->GetName() << "-int";
	TH3D *h = (TH3D*)h3->Clone(ssname.str().c_str());
	uint32_t nBinsX = h->GetNbinsX();
	uint32_t nBinsY = h->GetNbinsY();
	uint32_t nBinsZ = h->GetNbinsZ();
	for(uint32_t ibinx=1; ibinx<=nBinsX; ibinx++){
		std::cout << ibinx << " " << nBinsX << std::endl;
		for(uint32_t ibiny=1; ibiny<=nBinsY; ibiny++){
			for(uint32_t ibinz=1; ibinz<=nBinsZ; ibinz++){
				if(AreSame(h->GetBinContent(ibinx, ibiny, ibinz), 0, epsilon)){
					// Find closest filled x-bin towards left
					int32_t ibinxl = -1;
					double vall = 0;
					for(uint32_t bxl=ibinx-1; bxl>0; bxl--){
						vall = h->GetBinContent(bxl, ibiny, ibinz);

						if(!AreSame(vall, 0, epsilon)){
							ibinxl = bxl; // Found the closest filled bin on the left, save it for external use
							break;
						}
					}
					// Find closest filled x-bin towards right
					int32_t ibinxr = -1;
					double valr = 0;
					for(uint32_t bxr=ibinx+1; bxr<=nBinsX; bxr++){
						valr = h->GetBinContent(bxr, ibiny, ibinz);
						if(!AreSame(valr, 0, epsilon)){
							ibinxr = bxr; // Found the closest filled bin on the right, save it for external use
							break;
						}
					}
					if((ibinxr>0) && (ibinxl>0)){ // Only interpolate, don't extrapolate
						// Interpolate linearly
						double a = (valr-vall) / (ibinxr-ibinxl); // derivative-like -> how much val increases with each bin
						double val = vall + a*(ibinx-ibinxl);
						h->SetBinContent(ibinx, ibiny, ibinz, val);
					}
				}
			}
		}
	}
	return h;
}

/*TGraph* common::TransformZenithToHeightProfile(TH1D* hzenith){
	std::stringstream ss;
	ss << hzenith->GetName() << "_height" << std::endl;
	TGraph *gr = new TGraph(90);

	// Muon survival probability function for ~200 GeV muons (according to Kai's plot)
	TF1* fprob = new TF1("fprob","[0]+[1]*x+[2]*x^2",0,30e3);
	// Params extracted for initial energy of muons of 600 GeV
	fprob->SetParameter(0,1.0);
	fprob->SetParameter(1,-2.05446e-05);
	fprob->SetParameter(2,8.48992e-11);

	// Initial muon angular distribution (includes the 'perSr' unit)
	TF1* finit = new TF1("finit","([0]*cos(x*TMath::DegToRad())*cos(x*TMath::DegToRad()))",0,90);
	finit->SetParameter(0,9e6);
	TH1D* hinit = new TH1D("hinit","hinit",90,0,90);
	hinit->Add(finit);

	// Percentage of muons that reached the detector (normalized approximately to 20% at 42 degrees)
	hzenith->Divide(hinit);
	hzenith->Draw("");

	std::cout.precision(5);
	uint32_t nBinsX = hinit->GetXaxis()->GetNbins();
	for(uint32_t ibininit=1; ibininit<=nBinsX; ibininit++){
		double zenith = hinit->GetBinCenter(ibininit);
		double distance = hinit->GetBinContent(ibininit);
		if(distance>0){
			double length = distance*TMath::Sin(zenith*TMath::DegToRad());
			double height = distance*TMath::Cos(zenith*TMath::DegToRad());
			gr->SetPoint(ibininit-1,length,height);
		}
	}
	return gr;
}
*/
double common::GetXFromTH2(TH2* h2, const double y, const double z, const double epsilon){
	int16_t biny = h2->GetYaxis()->FindBin(y);
	TH1* h = h2->ProjectionX("hpy",biny,biny);
	TAxis* ax = h2->GetXaxis();
	// First find the two x-bins, between which z falls
	double x0=0;
	double x1=0;
	for(int16_t xbin=1; xbin<=ax->GetNbins(); xbin++){
		if(h->GetBinContent(xbin) > z){
			x0 = ax->GetBinLowEdge(xbin-1);
			x1 = ax->GetBinLowEdge(xbin+1);
			break;
		}
	}
	// Use interpolation to find exact x-value between these two bins
	double eps = 1e3;
	double x =0; // best interpolated value
	int cnt = 0; // number of iterations
	double preveps = 1e6;
	while(eps > epsilon){
		preveps = eps;
		x = x0 + 0.5*(x1-x0);
		double zx = h->Interpolate(x);
		if(zx > z){
			x1 = x;
		}
		else{
			x0 = x;
		}
		eps = fabs(z-zx);
		if(fabs(preveps-eps) < 1e-8){ // there is no significant change in epsilon anymore
			std::cout.precision(4);
			int width = 8;
			std::cout << " <W> common::GetXFromTH2(): Minimizer broke, epsilon no longer minimizes... " << std::endl;
			std::cout << "             ";
			std::cout << "y="    << std::left << std::setw(width) << y;
			std::cout << "cnt = "<< std::left << std::setw(width) << cnt;
			std::cout << "x0 = " << std::left << std::setw(width) << x0;
			std::cout << "x = "  << std::left << std::setw(width) << x;
			std::cout << "x1 = " << std::left << std::setw(width) << x1;
			std::cout << "zx = " << std::left << std::setw(width) << zx;
			std::cout << "z = "  << std::left << std::setw(width) << z;
			std::cout << "eps="     << std::left << std::setw(width) << eps;
			std::cout << "preveps=" << std::left << std::setw(width) << preveps << std::endl;
			break;
		}
		if(cnt++>1e3){
			std::cout << "y="    << y << " ";
			std::cout << "cnt = "<< cnt << " ";
			std::cout.precision(8);
			int width = 12;
			std::cout << "x0 = " << std::left << std::setw(width) << x0;
			std::cout << "x = "  << std::left << std::setw(width) << x;
			std::cout << "x1 = " << std::left << std::setw(width) << x1;
			std::cout << "zx = " << std::left << std::setw(width) << zx;
			std::cout << "z = "  << z << " ";
			std::cout.precision(3);
			width = 9;
			std::cout << "eps="     << std::left << std::setw(width) << eps << " ";
			std::cout << "preveps=" << std::left << std::setw(width) << preveps << std::endl;
			break;
		}
	}
	return x;
}

int common::ProduceAsciiFromTH2D(TH2D* h2, ostream& out){
	double nx = h2->GetNbinsX();
	double ny = h2->GetNbinsY();
	for(int16_t xbin=1; xbin<=nx; xbin+=h2->GetXaxis()->GetBinWidth(xbin)){
		for(int16_t ybin=1; ybin<=ny; ybin+=h2->GetYaxis()->GetBinWidth(ybin)){
			double x = h2->GetXaxis()->GetBinLowEdge(xbin);
			double y = h2->GetYaxis()->GetBinLowEdge(ybin);
			out << x << " " << y << " " << h2->GetBinContent(xbin,ybin) << std::endl;
		}
	}
	return 0;
}

void common::Multiply(TH2D* h, double f){
	for(uint16_t ibin=0; ibin<h->GetSize(); ibin++){
		h->SetBinContent(ibin, h->GetBinContent(ibin)*f);
		h->SetBinError(ibin, h->GetBinError(ibin)*f);
	}
}

void common::SetupRootStyle(){
	// Setup style, colors, etc.
	int icolor = 1000;
	int nc = 2;
	double dc = 1.0/nc;
	for(int ic=1; ic<nc; ic++){
		new TColor(icolor++, 1.0, 1.0-ic*dc, 0.0);
	}
	for(int ic=1; ic<nc; ic++){
		new TColor(icolor++, 1.0, 0.0, ic*dc);
	}
	for(int ic=1; ic<nc; ic++){
		new TColor(icolor++, 1.0-ic*dc, 0.0, 1.0);
	}
	for(int ic=1; ic<nc; ic++){
		new TColor(icolor++, 0.0, ic*dc, 1.0);
	}
	for(int ic=1; ic<nc; ic++){
		new TColor(icolor++, 0.0, 1.0, 1.0-ic*dc);
	}
	for(int ic=1; ic<nc; ic++){
		new TColor(icolor++, ic*dc, 1.0, 0.0);
	}
	gStyle->SetOptStat("ioue");
}

void common::DrawAndAdjust(TH1* h, std::string option, float x1ndc, float y1ndc, float x2ndc, float y2ndc, float linewidth, Color_t col){
	h->SetLineWidth(linewidth);
	h->SetLineColor(col);
	h->GetYaxis()->SetTitleOffset(1.8);
	h->GetZaxis()->SetTitleOffset(1.3);
	h->Draw(option.c_str());
	gPad->Update();
	TPaletteAxis *palette = (TPaletteAxis*)h->GetListOfFunctions()->FindObject("palette");
	if(palette != NULL){
		palette->SetX1NDC(0.88);
		palette->SetX2NDC(0.91);
	}
	TPaveStats *st = (TPaveStats*)h->FindObject("stats");
	if(st!=NULL){
		st->SetX1NDC(x1ndc);
		st->SetX2NDC(x2ndc);
		st->SetY1NDC(y1ndc);
		st->SetY2NDC(y2ndc);
		st->SetTextColor(col);
	}
	gPad->Modified();
}

void common::DrawWithTwoAxes(unsigned int ipad, TH1* h1, TH1* h2, Float_t xmin, Float_t xmax, Float_t ymin1, Float_t ymax1, Float_t ymin2, Float_t ymax2, float legx1, float legy1, float legx2, float legy2, int linewidth1, int linewidth2, bool logScale){
	// Setup Legend
	TLegend *leg = new TLegend();
	leg = new TLegend(legx1,legy1,legx2-gPad->GetCanvas()->GetRightMargin(),legy2);
	leg->AddEntry(h1, h1->GetTitle(), "l");
	leg->AddEntry(h2, h2->GetTitle(), "l");

	gPad->GetCanvas()->cd(ipad);
	gStyle->SetOptStat("");
    h1->GetXaxis()->SetRangeUser(xmin,xmax);
    h1->GetYaxis()->SetRangeUser(ymin1,ymax1);
	h1->SetLineWidth(linewidth1);
	h1->SetLineColor(kBlack);
	h1->SetTitle("");
	h2->GetXaxis()->SetRangeUser(xmin,xmax);
	h2->GetYaxis()->SetRangeUser(ymin2,ymax2);
	h2->SetLineWidth(linewidth2);
	h2->SetLineColor(kBlue);
	h2->SetTitle("");

	std::stringstream sspad, ssovl;
	sspad << "pad_" << h1->GetName() << "_" << h2->GetName();
	ssovl << "ovl_" << h1->GetName() << "_" << h2->GetName();
//	TCanvas *c1 = new TCanvas(sscvs.str().c_str(),sscvs.str().c_str(),1800,950);
	TPad *pad = new TPad(sspad.str().c_str(),"",0,0,1,1);
	pad->SetGrid();
	pad->SetLogy(logScale);
	pad->Draw();
	pad->cd();
	h1->GetYaxis()->SetLabelSize(1.5*h1->GetXaxis()->GetLabelSize());
	h1->SetYTitle("Background trigger rate [Hz]");
	h1->Draw("");

	//create a transparent pad drawn on top of the main pad
	pad->GetCanvas()->cd(ipad);
	TPad *overlay = new TPad(ssovl.str().c_str(),"",0,0,1,1);
	overlay->SetFillStyle(4000);
	overlay->SetFillColor(0);
	overlay->SetFrameFillStyle(4000);
	overlay->Draw();
	overlay->cd();
	overlay->DrawFrame(xmin,ymin2,xmax,ymax2);
	h2->Draw("A");
	//Draw an axis on the right side
	TGaxis *axis = new TGaxis(xmax,ymin2, xmax,ymax2, ymin2,ymax2, 510,"+L");
	axis->SetLineColor(kBlue);
	axis->SetLabelColor(kBlue);
	axis->SetTitleColor(kBlue);
	axis->SetTitleFont(gStyle->GetTitleFont("x"));
	axis->SetLabelFont(gStyle->GetLabelFont("x"));
	axis->SetTitle(h2->GetYaxis()->GetTitle());
	axis->SetTitleOffset(1.2);
	axis->Draw();

	leg->Draw();
}

void common::DrawWithTwoAxes2(unsigned int ipad, TH1* h1, TH1* h2, Float_t xmin, Float_t xmax, Float_t ymin1, Float_t ymax1, Float_t ymin2, Float_t ymax2, float legx1, float legy1, float legx2, float legy2, int linewidth1, int linewidth2, bool logScale){
	// Setup Legend
	TLegend *leg = new TLegend();
	leg = new TLegend(legx1,legy1,legx2-gPad->GetCanvas()->GetRightMargin(),legy2);
	leg->AddEntry(h1, h1->GetTitle(), "le");
	leg->AddEntry(h2, h2->GetTitle(), "le");
	leg->SetMargin(0.07);

	TPad* mainpad = (TPad*)gPad->GetCanvas()->cd(ipad);
	mainpad->cd();
	gStyle->SetOptStat("");
    h1->GetXaxis()->SetRangeUser(xmin,xmax);
    h1->GetYaxis()->SetRangeUser(ymin1,ymax1);
	h1->SetLineWidth(linewidth1);
	h1->SetLineColor(kBlue);
	h1->SetTitle("");
	h2->GetXaxis()->SetRangeUser(xmin,xmax);
	h2->GetYaxis()->SetRangeUser(ymin2,ymax2);
	h2->SetLineWidth(linewidth2);
	h2->SetLineColor(kBlack);
	h2->SetTitle("");

	std::stringstream sspad, ssovl;
	sspad << "pad_" << h1->GetName() << "_" << h2->GetName();
	ssovl << "ovl_" << h1->GetName() << "_" << h2->GetName();
//	TCanvas *c1 = new TCanvas(sscvs.str().c_str(),sscvs.str().c_str(),1800,950);
//	TPad *pad = new TPad(sspad.str().c_str(),"",mainpad->GetLeftMargin(),mainpad->GetBottomMargin(),1.0-mainpad->GetRightMargin(),1.0-mainpad->GetTopMargin());
	TPad *pad = new TPad(sspad.str().c_str(),"",0,0,1,1);
	pad->SetBottomMargin(mainpad->GetBottomMargin());
	pad->SetTopMargin(mainpad->GetTopMargin());
	pad->SetRightMargin(mainpad->GetRightMargin());
	pad->SetLeftMargin(mainpad->GetLeftMargin());
	pad->SetGrid();
	pad->SetLogy(logScale);
	pad->Draw();
	pad->cd();
	h1->DrawCopy("hist ac");
	h1->SetLineWidth(2.5);
	h1->SetLineColor(kBlue+2);
	h1->DrawCopy("same ae");

	//create a transparent pad drawn on top of the main pad
	pad->GetCanvas()->cd(ipad);
//	TPad *overlay = new TPad(ssovl.str().c_str(),"",mainpad->GetLeftMargin(),mainpad->GetBottomMargin(),1.0-mainpad->GetRightMargin(),1.0-mainpad->GetTopMargin());
	TPad *overlay = new TPad(ssovl.str().c_str(),"",0,0,1,1);
	overlay->SetFillStyle(4000);
	overlay->SetFillColor(0);
	overlay->SetFrameFillStyle(4000);
	overlay->SetBottomMargin(mainpad->GetBottomMargin());
	overlay->SetTopMargin(mainpad->GetTopMargin());
	overlay->SetRightMargin(mainpad->GetRightMargin());
	overlay->SetLeftMargin(mainpad->GetLeftMargin());
	overlay->Draw();
	overlay->cd();
	overlay->DrawFrame(xmin,ymin2,xmax,ymax2);
	gPad->SetGridx();
	gPad->SetGridy();
	h2->Draw("c");
	//Draw an axis on the right side
	TGaxis *axis = new TGaxis(xmax,ymin2, xmax,ymax2, ymin1,ymax1, 510,"+L");
	axis->SetLineColor(h1->GetLineColor());
	axis->SetLabelColor(h1->GetLineColor());
	axis->SetTitleColor(h1->GetLineColor());
	axis->SetTitleFont(gStyle->GetTitleFont("x"));
	axis->SetLabelFont(gStyle->GetLabelFont("x"));
	axis->SetTitle(h1->GetYaxis()->GetTitle());
	axis->SetLabelSize(h1->GetYaxis()->GetLabelSize());
	axis->SetTitleSize(h1->GetYaxis()->GetTitleSize());
	axis->SetTitleOffset(h1->GetYaxis()->GetTitleOffset());
	axis->Draw();

	leg->Draw();
}

void common::DrawWithTwoAxesMulti(unsigned int ipad, std::vector<TH1*> vh1, std::vector<TH1*> vh2, Float_t xmin, Float_t xmax, Float_t ymin1, Float_t ymax1, Float_t ymin2, Float_t ymax2, bool logScale){
	TPad* mainpad = (TPad*)gPad->GetCanvas()->cd(ipad);
	mainpad->cd();
	gStyle->SetOptStat("");
    vh1.at(0)->GetXaxis()->SetRangeUser(xmin,xmax);
    vh1.at(0)->GetYaxis()->SetRangeUser(ymin1,ymax1);
    vh1.at(0)->SetTitle("");
	vh2.at(0)->GetXaxis()->SetRangeUser(xmin,xmax);
	vh2.at(0)->GetYaxis()->SetRangeUser(ymin2,ymax2);
	vh2.at(0)->SetTitle("");

	std::stringstream sspad, ssovl;
	sspad << "pad_" << vh1.at(0)->GetName() << "_" << vh2.at(0)->GetName();
	ssovl << "ovl_" << vh1.at(0)->GetName() << "_" << vh2.at(0)->GetName();
	TPad *pad = new TPad(sspad.str().c_str(),"",0,0,1,1);
	pad->SetBottomMargin(mainpad->GetBottomMargin());
	pad->SetTopMargin(mainpad->GetTopMargin());
	pad->SetRightMargin(mainpad->GetRightMargin());
	pad->SetLeftMargin(mainpad->GetLeftMargin());
	pad->SetGrid();
	pad->SetLogy(logScale);
	pad->Draw();
	pad->cd();

	for(uint16_t ih=0; ih<vh1.size(); ih++){
		if(ih==0){
			vh1.at(ih)->DrawCopy("hist ac");
		}
		else{
			vh1.at(ih)->DrawCopy("same hist ac");
		}
		vh1.at(ih)->DrawCopy("same ae");
	}

	//create a transparent pad drawn on top of the main pad
	pad->GetCanvas()->cd(ipad);
//	TPad *overlay = new TPad(ssovl.str().c_str(),"",mainpad->GetLeftMargin(),mainpad->GetBottomMargin(),1.0-mainpad->GetRightMargin(),1.0-mainpad->GetTopMargin());
	TPad *overlay = new TPad(ssovl.str().c_str(),"",0,0,1,1);
	overlay->SetFillStyle(4000);
	overlay->SetFillColor(0);
	overlay->SetFrameFillStyle(4000);
	overlay->SetBottomMargin(mainpad->GetBottomMargin());
	overlay->SetTopMargin(mainpad->GetTopMargin());
	overlay->SetRightMargin(mainpad->GetRightMargin());
	overlay->SetLeftMargin(mainpad->GetLeftMargin());
	overlay->Draw();
	overlay->cd();
	overlay->DrawFrame(xmin,ymin2,xmax,ymax2);
	gPad->SetGridx();
	gPad->SetGridy();

	for(uint16_t ih=0; ih<vh2.size(); ih++){
		if(ih==0){
			vh2.at(ih)->DrawCopy("c");
		}
		else{
			vh2.at(ih)->DrawCopy("same c");
		}
	}

	//Draw an axis on the right side
	TGaxis *axis = new TGaxis(xmax,ymin2, xmax,ymax2, ymin1,ymax1, 510,"+L");
	axis->SetLineColor(vh2.at(0)->GetLineColor());
	axis->SetLabelColor(vh2.at(0)->GetLineColor());
	axis->SetTitleColor(vh2.at(0)->GetLineColor());
	axis->SetTitleFont(gStyle->GetTitleFont("x"));
	axis->SetLabelFont(gStyle->GetLabelFont("x"));
	axis->SetTitle(vh2.at(0)->GetYaxis()->GetTitle());
	axis->SetLabelSize(vh2.at(0)->GetYaxis()->GetLabelSize());
	axis->SetTitleSize(vh2.at(0)->GetYaxis()->GetTitleSize());
	axis->SetTitleOffset(vh2.at(0)->GetYaxis()->GetTitleOffset());
	axis->Draw();
}

void common::SetBinContentAndBinomialError(TH1* h, int32_t ibin, float nTrg, float N, float multiplFactor){
	h->SetBinContent(ibin, multiplFactor * nTrg/N);
	double error = multiplFactor * sqrt(nTrg*(1-nTrg/N))/N;
	//std::cout << nTrg << "-" << N << "    *" << multiplFactor << " = " << multiplFactor*nTrg/N << " " << error << std::endl;
	h->SetBinError(ibin, error);
}

void common::ArrangeStatBox(TH1* h1, int i, float x0, float y0, float w, float h){
	TPaveStats *st = (TPaveStats*)h1->FindObject("stats");
	if(st!=NULL){
		st->SetX1NDC(x0);
		st->SetX2NDC(x0-w);
		st->SetY1NDC(y0-i*w);
		st->SetY2NDC(y0-(i+1)*w);
		st->SetTextColor(h1->GetLineColor());
	}
	gPad->Modified();
	gPad->Update();
}

float common::AxisCoordsToNDC(TAxis *a, float x){
	int nbins = a->GetNbins();
	float x1 = a->GetBinLowEdge(1);
	float x2 = a->GetBinUpEdge(nbins);
	float f1 = gPad->GetLeftMargin();
	float f2 = 1.0-gPad->GetRightMargin();
	//std::cout << f1 << "," << f2 << "\t" << x1 << "," << x2 << std::endl;
	if(x<x1){
		std::cout << " <W> common::AxisCoordsToNDC(): x-input below x1: " << x << " " << x1 << std::endl;
		return f1;
	}
	else if(x>x2){
		std::cout << " <W> common::AxisCoordsToNDC(): x-input above x2: " << x << " " << x2 << std::endl;
		return f2;
	}
	else{
		return f1 + (x-x1)*(f2-f1)/(x2-x1);
	}
	return -1;
}

void common::DrawTextNdc(std::string s, double x, double y, double size, Color_t col, Float_t tangle){
	TLatex *t = new TLatex(0,0,s.c_str());
	t->SetTextAngle(tangle);
	t->SetNDC();
	t->SetX(x);
	t->SetY(y);
	t->SetTextSize(size);
	t->SetTextColor(col);
	t->Draw();
}

void common::DrawTextUser(std::string s, double x, double y, double size, Color_t col, Float_t tangle){
	TLatex *t = new TLatex(0,0,s.c_str());
	t->SetTextAngle(tangle);
	t->SetX(x);
	t->SetY(y);
	t->SetTextSize(size);
	t->SetTextColor(col);
	t->Draw();
}

void common::Sqrt(TH2D* h2){
	for(int16_t phi=-180; phi<180; phi++){
		for(int16_t theta=89; theta>=0; theta--){
			uint32_t bin = FindBin(h2,phi,theta);
			h2->SetBinContent(bin, sqrt(h2->GetBinContent(bin)));
		}
	}
}

void common::FillUntouchedBins(TH2D* h2, double value){
	double nx = h2->GetNbinsX();
	double ny = h2->GetNbinsY();
	for(int16_t xbin=1; xbin<=nx; xbin+=h2->GetXaxis()->GetBinWidth(xbin)){
		for(int16_t ybin=1; ybin<=ny; ybin+=h2->GetYaxis()->GetBinWidth(ybin)){
			Int_t bin = h2->GetBin(xbin,ybin);
			if(h2->GetBinContent(bin)==0){
				h2->SetBinContent(bin,value);
			}
		}
	}
}

void common::SetupPad(TH1* h, float fontSize, float lmargin, float rmargin, float tmargin, float bmargin, float xoffset, float yoffset, float zoffset){
	gPad->SetTopMargin(tmargin);
	gPad->SetBottomMargin(bmargin);
	gPad->SetLeftMargin(lmargin);
	gPad->SetRightMargin(rmargin);
	gPad->SetGridx();
	gPad->SetGridy();
	gPad->SetCrosshair();
	h->GetXaxis()->SetLabelSize(fontSize);
	h->GetXaxis()->SetTitleSize(fontSize);
	h->GetYaxis()->SetLabelSize(fontSize);
	h->GetYaxis()->SetTitleSize(fontSize);
	h->GetZaxis()->SetLabelSize(fontSize);
	h->GetZaxis()->SetTitleSize(fontSize);
	h->GetXaxis()->SetTitleOffset(xoffset);
	h->GetYaxis()->SetTitleOffset(yoffset);
	h->GetZaxis()->SetTitleOffset(zoffset);
}

double common::CalculateSolidAngle(double x, double y, double z){
	// Assume bottom side of detector is at 0 and top at z
	TRandom3 rnd;
	rnd.SetSeed(0);
	TF1 *fcos = new TF1("fcos", "TMath::Cos(x)",0,TMath::PiOver2());
	int nSuccesses = 0;
	int nTrials = 1e7;
	for(int i=0; i<nTrials; i++){
		// Randomize angles
		double phi = rnd.Uniform(2.*TMath::Pi())-TMath::Pi();
		double theta = fcos->GetRandom();
		// Randomize start position offset from the center at the bottom detector
		double xb = rnd.Uniform(x);
		double yb = rnd.Uniform(y);
		// Calculate the x,y coordinate of the track at the z of the top face of the detector
		double xt = xb + z*TMath::Sin(phi);
		double yt = yb + z*TMath::Sin(phi)*TMath::Tan(theta);

		//std::cout << phi*TMath::RadToDeg() << ", ";
		//std::cout << theta*TMath::RadToDeg() << ", ";
		//std::cout << tx1 << ", ";
		//std::cout << tx2 << ", ";
		//std::cout << ty1 << ", ";
		//std::cout << ty2 << ", ";
		// Check if the track projection crosses detectors
		if((xt >= 0) && (xt <= x)){
			if((yt >= 0) && (yt <= y)){
				nSuccesses++;
			}
		}
	}

	// half-sphere to-steradian-conversion because only half of the sphere was scanned (zenith 0-90)
	return 2.*TMath::Pi()*nSuccesses/nTrials;
}


double common::CalculateSolidAngle(double *x0, double *y0, double *z0, double *dx, double *dy, const int n){
// In case n==2
//         ______________________
//         \                     \  dy[0]
//  z0[0]   \(x0[0],y0[0])________\.
//                    dx[0]
//         ___________________
//         \                  \  dy[1]
//   z0[1]  \(x0[1],y0[1])_____\.
//                 dx[1]
//
	if(n<1){
		std::cout << "<E> common::CalculateSolidAngle():Wrong n=" << n << std::endl;
		return -1;
	}
	TRandom3 rnd;
	rnd.SetSeed(0);
	TF1 *fcos2 = new TF1("fcos2", "TMath::Power(TMath::Sin(x)*TMath::Cos(x),1)",0,TMath::PiOver2());
	int nSuccesses = 0;
	int nTrials = 1e7;
	for(int itrial=0; itrial<nTrials; itrial++){
		// Randomize angles
		double phi = rnd.Uniform(2.0*TMath::Pi());
		double theta = fcos2->GetRandom();
		// Randomize start position offset from the (x0, y0) of the bottom detector
		double rndx = x0[0] + rnd.Uniform(dx[0]);
		double rndy = y0[0] + rnd.Uniform(dy[0]);
		bool allLevelsHit = true;
		// Loop over z-levels
		for(int iz=1; iz<n; iz++){
			// Calculate the x,y coordinate of the track at the z of the top (last) rectangle
			double dz = z0[iz]-z0[0];
			double x = rndx + dz*TMath::Tan(theta)*TMath::Cos(phi);
			double y = rndy + dz*TMath::Tan(theta)*TMath::Sin(phi);
			// If level is not hit change the flag to false
			if((x<x0[iz]) || (x>(x0[iz]+dx[iz]))){
				allLevelsHit = false;
				break;
			}
			if((y<y0[iz]) || (y>(y0[iz]+dy[iz]))){
				allLevelsHit = false;
				break;
			}
		}
		if(allLevelsHit){
			nSuccesses++;
		}
	}
	// half-sphere to-steradian-conversion because only half of the sphere was scanned (zenith 0-90)
	return TMath::Pi()*nSuccesses/nTrials;
}

double common::CalculateSolidAngleTest(){
	TRandom3 rnd;
	rnd.SetSeed(0);
	TF1 *fcos = new TF1("fcos", "TMath::Power(TMath::Cos(x),1)",0,TMath::PiOver2());
	TF1 *fsin = new TF1("fsin", "TMath::Power(TMath::Sin(x),1)",0,TMath::PiOver2());
//	TF1 *ftan = new TF1("ftan", "TMath::Power(TMath::Tan(x),1)",0,TMath::PiOver2());
	int nSuccesses = 0;
	int nTrials = 1e7;
	TH2D* h1 = new TH2D("h1","h1",360,0,360,90,0,90);
	TH2D* h2 = new TH2D("h2","h2",360,0,360,90,0,90);
	TH2D* h3 = new TH2D("h3","h3",360,0,360,90,0,90);
	TH2D* h4 = new TH2D("h4","h4",360,0,360,90,0,90);
	TH2D* h5 = new TH2D("h5","h5",180,-90,90,180,-90,90);
	TH2D* h6 = new TH2D("h6","h6",180,-90,90,180,-90,90);
	TH2D* h7 = new TH2D("h7","h7",180,-90,90,180,-90,90);
	TH2D* h8 = new TH2D("h8","h8",180,-90,90,180,-90,90);
	for(int itrial=0; itrial<nTrials; itrial++){
		// Randomize angles
		double phi = rnd.Uniform(2.0*TMath::Pi());
		double theta = rnd.Uniform(TMath::PiOver2());
		double phideg = TMath::RadToDeg()*phi;
		double thetadeg = TMath::RadToDeg()*theta;
		h1->Fill(phideg, thetadeg);

		theta = fsin->GetRandom();
		phideg = TMath::RadToDeg()*phi;
		thetadeg = TMath::RadToDeg()*theta;
		h2->Fill(phideg, thetadeg);

		theta = fcos->GetRandom();
		phideg = TMath::RadToDeg()*phi;
		thetadeg = TMath::RadToDeg()*theta;
		h3->Fill(phideg, thetadeg);

		theta = TMath::PiOver2()*TMath::Sqrt(rnd.Uniform());
		phideg = TMath::RadToDeg()*phi;
		thetadeg = TMath::RadToDeg()*theta;
		h4->Fill(phideg, thetadeg);

		if(itrial%100000==0){
			std::cout << (double)itrial/nTrials << std::endl;
		}
		if(theta<TMath::Pi()/3){
			nSuccesses++;
		}
	}
	TCanvas *cvs = new TCanvas("cvs","cvs",1800,900);
	cvs->Divide(4,2);
	cvs->cd(1);
		h1->Draw("colz");
	cvs->cd(2);
		h2->Draw("colz");
	cvs->cd(3);
		h3->Draw("colz");
	cvs->cd(4);
		h4->Draw("colz");
	cvs->cd(5);
		h5->Draw("colz");
		gPad->SetLogz();
	cvs->cd(6);
		h6->Draw("colz");
		gPad->SetLogz();
	cvs->cd(7);
		h7->Draw("colz");
		gPad->SetLogz();
	cvs->cd(8);
		h8->Draw("colz");
		gPad->SetLogz();

	// half-sphere to-steradian-conversion because only half of the sphere was scanned (zenith 0-90)
	return 2.*TMath::Pi()*nSuccesses/nTrials;
}
double common::CalculateSolidAngleFromSphere(double *x0, double *y0, double *z0, double *dx, double *dy, const int n){
/* In case n==2
 *         ______________________
 *         \                     \  dy[0]
 *  z0[0]   \(x0[0],y0[0])________\
 *                    dx[0]
 *         ___________________
 *         \                  \  dy[1]
 *   z0[1]  \(x0[1],y0[1])_____\
 *                 dx[1]
*/
	if(n<1){
		std::cout << "<E> common::CalculateSolidAngle():Wrong n=" << n << std::endl;
		return -1;
	}
	// Determine the position and size of the sphere from which the randomized muons will originate
	double minz = 999;
	double maxz = -999;
	double minx = 999;
	double maxx = -999;
	double miny = 999;
	double maxy = -999;
	for(int i=0; i<n; i++){
		if(z0[i] > maxz) maxz = z0[i];
		if(z0[i] < minz) minz = z0[i];
		if(x0[i]+dx[i] > maxx) maxx = x0[i]+dx[i];
		if(x0[i] < minx) minx = x0[i];
		if(y0[i]+dy[i] > maxy) maxy = y0[i]+dy[i];
		if(y0[i] < miny) miny = y0[i];
	}
	double x0Sphere = (minx + maxx) / 2;
	double y0Sphere = (miny + maxy) / 2;
	double z0Sphere = (minz + maxz) / 2;
	double rSphere = 1.001 * 0.5 * TMath::Sqrt(TMath::Power(maxx-minx, 2) + TMath::Power(maxy-miny,2) + TMath::Power(maxz-minz,2));
	rSphere = 1.1 * 10;

	TRandom3 rnd;
	rnd.SetSeed(0);
	TF1 *fcos = new TF1("fcos", "TMath::Power(TMath::Cos(x),1)",0,TMath::PiOver2());
	int nSuccesses = 0;
	int nTrials = 1e8;
	TH2D* h2 = new TH2D("h2","h2",100,-1,4,100,-1,4);
	for(int itrial=0; itrial<nTrials; ){
		// Randomize angles
		double phi = rnd.Uniform(2.*TMath::Pi())-TMath::Pi();
		double theta = fcos->GetRandom();
		// Randomize start position offset from the (x0, y0) of the bottom detector
		double xSphere, ySphere, zSphere;
		rnd.Sphere(xSphere, ySphere, zSphere, rSphere);
		if(zSphere>=0){ // Limit the sim to only top half-sphere
			itrial++;
			xSphere += x0Sphere;
			ySphere += y0Sphere;
			zSphere += z0Sphere;
			bool allLevelsHit = true;
			// Loop over z-levels
			for(int iz=0; iz<n; iz++){
				double dz = z0[iz]-zSphere;
				double x = xSphere + dz*TMath::Sin(phi);
				double y = ySphere + dz*TMath::Sin(phi)*TMath::Tan(theta);
				h2->Fill(x,y);
				// If level is not hit -> change the flag to false
				if((x<x0[iz]) || (x>(x0[iz]+dx[iz]))){
					allLevelsHit = false;
					break;
				}
				if((y<y0[iz]) || (y>(y0[iz]+dy[iz]))){
					allLevelsHit = false;
					break;
				}
			}
			if(allLevelsHit){
				nSuccesses++;
			}
		}
	}

	h2->Draw("colz");
	// half-sphere to-steradian-conversion because only half of the sphere was scanned (zenith 0-90)
	return (double)nSuccesses/nTrials;

	// ===========================================
	// Example of Solid angle comparison betwen part of MM and the additional scintillators on top
	const int n1=3;
	double x1[n1] =  { 0.0, 0.5, 0.0};
	double y1[n1] =  { 0.0, 0.5, 0.0};
	double z1[n1] =  { 0.346, 0.18, 0.0};
	double dx1[n1] = { 1.5, 0.5, 1.5};
	double dy1[n1] = { 1.5, 0.5, 1.5};
	std::cout << "MMpart = " << common::CalculateSolidAngleFromSphere(x1,y1,z1,dx1,dy1,n1) / (2.*TMath::Pi()) << std::endl;
	const int n2=2;
	double x2[n2] =  { 0.0, 0.0};
	double y2[n2] =  { 0.0, 0.0};
	double z2[n2] =  { 0.0, 0.016};
	double dx2[n2] = { 1.0, 1.0};
	double dy2[n2] = { 0.5, 0.5};
	std::cout << "AddSc1.6  = " << common::CalculateSolidAngleFromSphere(x2,y2,z2,dx2,dy2,n2) / (2.*TMath::Pi()) << std::endl;
	z2[1]=0.05;
	std::cout << "AddSc5.0  = " << common::CalculateSolidAngleFromSphere(x2,y2,z2,dx2,dy2,n2) / (2.*TMath::Pi()) << std::endl;
	z2[1]=0.1;
	std::cout << "AddSc10  = " << common::CalculateSolidAngleFromSphere(x2,y2,z2,dx2,dy2,n2) / (2.*TMath::Pi()) << std::endl;
	z2[1]=0.15;
	std::cout << "AddSc15  = " << common::CalculateSolidAngleFromSphere(x2,y2,z2,dx2,dy2,n2) / (2.*TMath::Pi()) << std::endl;
	// ===========================================
}
