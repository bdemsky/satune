#ifndef NAIVEELEMENTENCODER_H
#define NAIVEELEMENTENCODER_H
#include "classlist.h"
#include "structs.h"



/**
 *For now, This function just simply goes through elements/functions and 
 *assigns a predefined Encoding to each of them 
 * @param csolver
 * @param encoder
 */
void naiveEncodingDecision(CSolver* csolver);
void baseBinaryIndexElementAssign(ElementEncoding *This);

#endif
