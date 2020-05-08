/*
 * TScMapReader.h
 *
 *  Created on: Dec 18, 2011
 *      Author: mss
 */

#ifndef TSCMAPREADER_H_
#define TSCMAPREADER_H_

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include "TSingleScMap.h"
#include "TObject.h"

using namespace std;

class TScMapReader : public TObject{
private:
	string					sScMap;
	string					firstline, secondline;
	vector<TSingleScMap>	scMap;

	int16_t maxXCoord;
	int16_t minXCoord;
	int16_t maxYCoord;
	int16_t minYCoord;
	int16_t maxZCoord;
	int16_t minZCoord;

	// Sizes of lutScPos vector in all 3 dimenstions (size of sc16 array)
	int16_t nXCoords;
	int16_t nYCoords;
	int16_t nZCoords;

	// if sc number is not initialised or it is incorrect these luts return value: N_SCMODULES-1
	int16_t lutLeftiSc[N_SCMODULES];
	int16_t lutRightiSc[N_SCMODULES];
	int16_t lutUpiSc[N_SCMODULES];
	int16_t lutDowniSc[N_SCMODULES];
	int16_t lutForwardiSc[N_SCMODULES];
	int16_t lutBackwardiSc[N_SCMODULES];

	// Positions of sc modules
	vector3d lutScPos;
	int16_t iXlutScPos[N_SCMODULES];
	int16_t iYlutScPos[N_SCMODULES];
	int16_t iZlutScPos[N_SCMODULES];

	// Positions of pixels
	vector3d lutZyxToScModule;
	vector3d lutZyxToScPixel;
	int32_t lutScModulePixelToZ[N_SCMODULES][N_PIXELS];
	int32_t lutScModulePixelToY[N_SCMODULES][N_PIXELS];
	int32_t lutScModulePixelToX[N_SCMODULES][N_PIXELS];

	void FillLutTables();
	int16_t FillLutSurroundingScPos();
	int16_t FillLutSurroundingiScs();
	void FillLutPixelPos();
	bool BelongsToSamePlane(uint16_t, uint16_t, char, char);

public:
	TScMapReader();
	void ResetMap();
	bool fileOpened;

	int16_t lutHbSc16[N_TDC_CHANNELS];
	int16_t lutTbSc16[N_TDC_CHANNELS];
	int16_t lutSc16Hb[N_SCMODULES];
	int16_t lutSc16Tb[N_SCMODULES];
	int16_t lutiSc16Pos[N_SCMODULES];

	static uint8_t SC1[N_PIXELS];
	static uint8_t iSC1[N_PIXELS];
	static uint8_t iSC1x[N_PIXELS];
	static uint8_t iSC1y[N_PIXELS];

	static uint8_t thetaFlipSC1[N_PIXELS];
	static uint8_t phiRot90ClkSC1[N_PIXELS];
	static uint8_t phiRot90AClkSC1[N_PIXELS];
	static uint8_t phiRot180SC1[N_PIXELS];
	static string SC1nToSC1Name[N_PIXELS];

	static uint16_t ConvSc1(uint8_t); //Converts (1a, 2a, 3a, 4a, 1b, 2b, ..., 3d, 4d) order to (0,1,2,..,15) - where 0 is upper left corner and 15 is lower right corner of SC16
	static uint16_t iConvSc1(uint8_t); //Inverted function: ConvSc1()
	static string GetSC1nToSC1Name(uint8_t);

	static uint16_t GetXg(const uint16_t x, const uint16_t xp);
	static uint16_t GetYg(const uint16_t y, const uint16_t yp);
	static uint16_t GetX(const uint16_t xg);
	static uint16_t GetY(const uint16_t yg);
	static uint16_t GetXp(const uint16_t xg);
	static uint16_t GetYp(const uint16_t yg);

	vector<TSingleScMap>* GetScMap();
	string* GetsScMap();
	void SetsScMap(string&);

	int ReadMapFile(string);
	int ReadMapString(string&);
	int PrintMap(ostream&);

	int ListPlaneCoord(vector<int16_t>&, char);

	int FindColumnList(vector<int16_t>& ColList, int32_t x, int32_t y);	//returns vector of indexes (ColList) of the detector column (x,y: coordinates)
	int FindPlane(vector<int16_t>& PlaneList, uint8_t zn);		 		//returns vector of indexes (PlaneList) of the detector plane (z=0: bottom, z=max: topmost)

	int16_t GetMaxXCoord();
	int16_t GetMinXCoord();
	int16_t GetMaxYCoord();
	int16_t GetMinYCoord();
	int16_t GetMaxZCoord();
	int16_t GetMinZCoord();

	int16_t GetLeftiSc(uint16_t imappos);
	int16_t GetRightiSc(uint16_t imappos);
	int16_t GetUpiSc(uint16_t imappos);
	int16_t GetDowniSc(uint16_t imappos);
	int16_t GetForwardiSc(uint16_t imappos);
	int16_t GetBackwardiSc(uint16_t imappos);

	vector3d* GetLutScPos();
	int16_t GetiLutScPos(uint16_t imappos, uint16_t& ix, uint16_t& iy, uint16_t& iz, bool printWarnings=true);

	vector3d* GetLutZyxToScModule();
	vector3d* GetLutZyxToScPixel();
	int32_t GetLutScModulePixelToZ(uint16_t scn, uint16_t ipixel);
	int32_t GetLutScModulePixelToY(uint16_t scn, uint16_t ipixel);
	int32_t GetLutScModulePixelToX(uint16_t scn, uint16_t ipixel);

	// Different coordinate systems (xg,yg - grid [=0..], x,y - sc16 coords [=0..], xp,yp - pixel coords [=0..3], ng - linear combination of grid coords [=0..])
	uint16_t GetNg(const uint16_t x, const uint16_t xp, const uint16_t y, const uint16_t yp);
	uint16_t GetNgFromGlobal(const uint16_t xg, const uint16_t yg, const uint16_t zg); // from global pixel coordinates (pixels mapped regardless of SC16s)
	void GetXxpYypFromNg(const uint16_t ng, uint16_t &x, uint16_t &xp, uint16_t &y, uint16_t &yp);
	void GetXgYgZgFromNgGlobal(const uint16_t ng, uint16_t &xg, uint16_t &yg, uint16_t &zg);

	int16_t GetRotatedPixelsCoords(uint16_t isc, uint16_t ipixel, uint16_t &xp, uint16_t &yp);

	ClassDef(TScMapReader,1);
};

#endif /* TSCMAPREADER_H_ */
