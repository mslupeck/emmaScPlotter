/*
 * TCuts.h
 *
 *  Created on: Dec 6, 2019
 *      Author: mss
 */

#ifndef TCUTS_H_
#define TCUTS_H_

class TCuts {
public:
	bool ignoreHitsWithoutTiming;
	bool ignoreHitsWithoutPattern;
	bool acceptHitsFromPromptPeakOnly;
	int promptT0; // start time of the prompt-peak gate in TDC channels (100-ps)
	int promptT1; // stop time of the prompt-peak gate in TDC channels (100-ps)

	TCuts();
	virtual ~TCuts();
};

#endif /* TCUTS_H_ */
