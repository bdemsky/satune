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

void computePolarities(CSolver *This);
void updatePolarity(Boolean *This, Polarity polarity);
void updateMustValue(Boolean *This, BooleanValue value);
void computePolarity(Boolean *boolean);
void computePredicatePolarity(BooleanPredicate *This);
void computeLogicOpPolarity(BooleanLogic *boolean);
Polarity negatePolarity(Polarity This);
BooleanValue negateBooleanValue(BooleanValue This);
void computeLogicOpPolarityChildren(BooleanLogic *boolean);

#endif/* POLARITYASSIGNMENT_H */
