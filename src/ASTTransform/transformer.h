/*
 * File:   transformer.h
 * Author: hamed
 *
 * Created on August 24, 2017, 5:33 PM
 */

#ifndef ORDERDECOMPOSE_H
#define ORDERDECOMPOSE_H
#include "classlist.h"
#include "structs.h"
#include "transform.h"
#include "integerencoding.h"

class Transformer{
public:
	Transformer(CSolver* solver);
	~Transformer();
	IntegerEncodingTransform* getIntegerEncodingTransform(){ return integerEncoding; }
	void orderAnalysis();
private:
	//For now we can just add transforms here, but in future we may want take a smarter approach.
	IntegerEncodingTransform* integerEncoding;
	
	
	CSolver* solver;
};


#endif/* ORDERDECOMPOSE_H */

