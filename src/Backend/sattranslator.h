/* 
 * File:   sattranslator.h
 * Author: hamed
 *
 * Created on July 11, 2017, 5:27 PM
 */

#ifndef SATTRANSLATOR_H
#define SATTRANSLATOR_H

#include "classlist.h"

bool getBooleanVariableValueSATTranslator( CSolver* This , Boolean* boolean);
uint64_t getElementValueBinaryIndexSATTranslator(CSolver* This, ElementEncoding* elemEnc);
uint64_t getElementValueBinaryValueSATTranslator(CSolver* This, ElementEncoding* elemEnc);
uint64_t getElementValueOneHotSATTranslator(CSolver* This, ElementEncoding* elemEnc);
uint64_t getElementValueUnarySATTranslator(CSolver* This, ElementEncoding* elemEnc);
uint64_t getElementValueSATTranslator(CSolver* This, Element* element);

#endif /* SATTRANSLATOR_H */

