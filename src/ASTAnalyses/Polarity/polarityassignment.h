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
#include "element.h"

void computePolarities(CSolver *This);
bool updatePolarity(Boolean *This, Polarity polarity);
void updateEdgePolarity(BooleanEdge dst, BooleanEdge src);
void updateEdgePolarity(BooleanEdge dst, Polarity polarity);
void updateMustValue(Boolean *This, BooleanValue value);
void computeElement(Element *e);
void computePolarity(Boolean *boolean, Polarity polarity);
void computePredicatePolarity(BooleanPredicate *This);
void computeLogicOpPolarity(BooleanLogic *boolean);
BooleanValue negateBooleanValue(BooleanValue This);
Polarity computeLogicOpPolarityChildren(BooleanLogic *boolean);

#endif/* POLARITYASSIGNMENT_H */
