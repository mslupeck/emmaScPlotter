/*
 * TTreeStorage.h
 *
 *  Created on: May 18, 2017
 *      Author: mss
 *  Classes used as event storage
 *  One object of TFileStorage class represents a single event
 *  One object of THitStorage class represents one scintillator hit
 *  One object of TFlagStorage class represents information about flags in this event
 */

#ifndef TFILESTORAGE_H_
#define TFILESTORAGE_H_

#include <TObject.h>
#include <vector>

class THitStorage : public TObject {
public:
	Int_t scModule;
	Int_t scPixel;
	Int_t x; // position [mm]
	Int_t y; // position [mm]
	Int_t z; // position [mm]
	Int_t t; // time [0.1 ns] (1 TDC channel is 0.1 ns)

	THitStorage();
	void Fill(Int_t scModule, Int_t scPixel, Int_t x, Int_t y, Int_t z, Int_t t);

	ClassDef(THitStorage, 1)
};

class TFlagStorage : public TObject {
public:
	// These are vectors because this is multihit TDC, ie.
	//  it is capable of registering more than one signal
	//  in the same channel during the same event
	std::vector<Int_t> ch; // channel number
	std::vector<Int_t> t;  // time in TDC channels (1 channel => 0.8 ns)

	TFlagStorage();
	void push_back(Int_t ch, Int_t t);

	ClassDef(TFlagStorage, 1)
};

class TFileStorage : public TObject {
public:
	Int_t iRunNumber;
	Int_t iFileNumber;
	Long64_t lFileStartTimeS;
	Long64_t lFileStartTimeNs;
	Float_t fFileDuration;
	Float_t fFileDurationUncertainty;
	Float_t fFileT0; // the time of the first event in a file [s]
	Double_t fEventTimeS; // the time of this event [s]
	std::vector<THitStorage> vHitPoint;
	std::vector<TFlagStorage> vFlag;

	TFileStorage();
	void Clear();

	ClassDef(TFileStorage, 1)
};

#endif /* TFILESTORAGE_H_ */
