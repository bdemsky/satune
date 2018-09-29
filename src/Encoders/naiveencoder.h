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
void naiveEncodingConstraint(CSolver *csolver, Boolean *This);
void naiveEncodingLogicOp(CSolver *csolver, BooleanLogic *This);
void naiveEncodingPredicate(CSolver *csolver, BooleanPredicate *This);
void naiveEncodingElement(CSolver *csolver, Element *This);
#endif
