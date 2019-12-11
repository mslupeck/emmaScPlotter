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
}

TCuts::~TCuts() {
	// TODO Auto-generated destructor stub
}

