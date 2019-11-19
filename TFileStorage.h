/*
 * TTreeStorage.h
 *
 *  Created on: May 18, 2017
 *      Author: mss
 */

#ifndef TFILESTORAGE_H_
#define TFILESTORAGE_H_

#include <TObject.h>
#include <vector>

class THitStorage : public TObject {
public:
	Int_t scModule;
	Int_t scPixel;
	Int_t x;
	Int_t y;
	Int_t z;
	Int_t t;

	THitStorage();
	void Fill(Int_t scModule, Int_t scPixel, Int_t x, Int_t y, Int_t z, Int_t t);

	ClassDef(THitStorage, 1)
};

class TFlagStorage : public TObject {
public:
	std::vector<Int_t> ch;
	std::vector<Int_t> t;

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
	Float_t fFileT0;
	Double_t fEventTimeS;
	std::vector<THitStorage> vHitPoint;
	std::vector<TFlagStorage> vFlag;

	TFileStorage();
	void Clear();

	ClassDef(TFileStorage, 1)
};

#endif /* TFILESTORAGE_H_ */
