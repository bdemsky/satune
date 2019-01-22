#ifndef ALLOYENC_H
#define ALLOYENC_H

#include "classlist.h"
#include "signatureenc.h"
#include <iostream>
#include <fstream>
using namespace std;

class AlloyEnc{
public:
	AlloyEnc(CSolver *solver);
	void encode();
	int solve();
	void writeToFile(string str);
	uint64_t getValue(Element * element);
	~AlloyEnc();
private:
	string encodeConstraint(BooleanEdge constraint);
	int getResult();
	string encodeBooleanLogic( BooleanLogic *bl);
	string encodePredicate( BooleanPredicate *bp);
	string encodeOperatorPredicate(BooleanPredicate *constraint);
	CSolver *csolver;
	SignatureEnc sigEnc;
	ofstream output;
	static const char * alloyFileName;
	static const char * solutionFile;
};

#endif
