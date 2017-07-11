#ifndef NAIVEELEMENTENCODER_H
#define NAIVEELEMENTENCODER_H
#include "classlist.h"
#include "structs.h"

/**
 * The NaiveEncoder assigns a predefined Encoding to each Element and Function.
 * @param csolver
 * @param encoder
 */
void naiveEncodingDecision(CSolver* csolver);
void baseBinaryIndexElementAssign(ElementEncoding *This);

#endif
