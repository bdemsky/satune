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
	uint64_t getValue(Element *element);
	bool getBooleanValue(Boolean *element);
	~AlloyEnc();
private:
	void dumpAlloyFooter();
	void dumpAlloyHeader();
	string encodeConstraint(BooleanEdge constraint);
	int getResult();
	string encodeBooleanLogic( BooleanLogic *bl);
	string encodeBooleanVar( BooleanVar *bv);
	string encodePredicate( BooleanPredicate *bp);
	string encodeOperatorPredicate(BooleanPredicate *constraint);
	string processElementFunction(ElementFunction *element, ElementSig *signature);
	CSolver *csolver;
	SignatureEnc sigEnc;
	ofstream output;
	static const char * alloyFileName;
	static const char * solutionFile;
};

#endif
