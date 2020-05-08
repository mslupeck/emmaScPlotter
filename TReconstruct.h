/*
 * TReconstruct.h
 *
 *  Created on: May 5, 2020
 *      Author: mss
 */

#include "EmmaEventTreeReader.h"
#include "TVisualize.h"

#ifndef TRECONSTRUCT_H_
#define TRECONSTRUCT_H_

namespace std {

class TReconstruct {
private:
	TVisualize visualizer;
	EmmaEventTreeReader *eetr;
	int verbose;

public:
	TReconstruct(EmmaEventTreeReader *eetr, int verbose);
	virtual ~TReconstruct();

	void RunReconstruct();
};

} /* namespace std */

#endif /* TRECONSTRUCT_H_ */
