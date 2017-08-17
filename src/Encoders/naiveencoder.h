#ifndef NAIVEELEMENTENCODER_H
#define NAIVEELEMENTENCODER_H
#include "classlist.h"
#include "structs.h"

/**
 * The NaiveEncoder assigns a predefined Encoding to each Element and Function.
 * @param csolver
 * @param encoder
 */

void naiveEncodingDecision(CSolver *csolver);
void naiveEncodingConstraint(Boolean *This);
void naiveEncodingLogicOp(BooleanLogic *This);
void naiveEncodingPredicate(BooleanPredicate *This);
void naiveEncodingElement(Element *This);
void encodingArrayInitialization(ElementEncoding *This);
uint getSizeEncodingArray(ElementEncoding *, uint setSize);

#endif
