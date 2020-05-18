/*
 * TCuts.cpp
 *
 *  Created on: Dec 6, 2019
 *      Author: mss
 */

#include "TCuts.h"

TCuts::TCuts() {
	ignoreHitsWithoutTiming = true;
	ignoreHitsWithoutPattern = true;
	acceptHitsFromPromptPeakOnly = true;
	promptT0 = -1e9;
	promptT1 = 1e9;
	n0 = -1;
	nMax = -1;
}

TCuts::TCuts(const TCuts *c) {
	ignoreHitsWithoutTiming = c->ignoreHitsWithoutTiming;
	ignoreHitsWithoutPattern = c->ignoreHitsWithoutPattern;
	acceptHitsFromPromptPeakOnly = c->acceptHitsFromPromptPeakOnly;
	promptT0 = c->promptT0;
	promptT1 = c->promptT1;
	n0 = c->n0;
	nMax = c->nMax;
}

TCuts::~TCuts() {
	// TODO Auto-generated destructor stub
}

