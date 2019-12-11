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
	int promptT0;
	int promptT1;

	TCuts();
	virtual ~TCuts();
};

#endif /* TCUTS_H_ */
