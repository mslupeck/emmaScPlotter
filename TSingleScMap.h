/*
 * TScMap.h
 *
 *  Created on: Dec 18, 2011
 *      Author: mss
 */

#ifndef TSINGLESCMAP_H_
#define TSINGLESCMAP_H_

#include <iostream>
#include <string>
#include <vector>
#include <string.h>
#include "Types.h"
#include "TObject.h"

using namespace std;

class TSingleScMap : public TObject{
public:
	TSingleScMap();
	virtual ~TSingleScMap();

	string place;
	string sSc16;
	int16_t iSc16;
	int16_t tbCh;
	int16_t hbCh;
	int16_t xCoord;
	int16_t yCoord;
	int16_t zCoord;
	int16_t theta;
	int16_t phi;

	bool CheckValidity();
	int8_t Print(ostream&);

	ClassDef(TSingleScMap,1);
};

#endif /* TSINGLESCMAP_H_ */
