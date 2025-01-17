/*
 * File:   sattranslator.h
 * Author: hamed
 *
 * Created on July 11, 2017, 5:27 PM
 */

#ifndef SATTRANSLATOR_H
#define SATTRANSLATOR_H

#include "classlist.h"
#include "ops.h"


bool getBooleanVariableValueSATTranslator( CSolver *This, Boolean *boolean);

/**
 * most significant bit is represented by variable index 0
 */
uint64_t getElementValueBinaryIndexSATTranslator(CSolver *This, ElementEncoding *elemEnc);
uint64_t getElementValueBinaryValueSATTranslator(CSolver *This, ElementEncoding *elemEnc);
uint64_t getElementValueOneHotSATTranslator(CSolver *This, ElementEncoding *elemEnc);
uint64_t getElementValueUnarySATTranslator(CSolver *This, ElementEncoding *elemEnc);
uint64_t getElementValueSATTranslator(CSolver *This, Element *element);

#endif/* SATTRANSLATOR_H */

