/* 
 * File:   polarityassignment.h
 * Author: hamed
 *
 * Created on August 6, 2017, 12:18 PM
 */

#ifndef POLARITYASSIGNMENT_H
#define POLARITYASSIGNMENT_H
#include "classlist.h"
#include "mymemory.h"
#include "common.h"
#include "ops.h"
#include "boolean.h"

void assignPolarityAndBooleanValue(Boolean* boolean);
void assignPolarityAndBooleanValue(Boolean* boolean);
void assignPredicatePolarityAndBooleanValue(Boolean* boolean);
void assignLogicOpPolarityAndBooleanValue(Boolean* boolean);
void computeLogicOpPolarity(Boolean* boolean);
void computeLogicOpBooleanValue(Boolean* boolean);
void computeImplicationBooleanValue(Boolean* first, Boolean* second, BooleanValue parent);
void computePolarityAndBooleanValue(Boolean* boolean);
void computePredicatePolarityAndBooleanValue(Boolean* boolean);
void computeLogicOpPolarityAndBooleanValue(Boolean* boolean);
BooleanValue computeBooleanValue(LogicOp op, BooleanValue childVal, BooleanValue parentVal );
Polarity computePolarity(Polarity childPol, Polarity parentPol);


#endif /* POLARITYASSIGNMENT_H */
