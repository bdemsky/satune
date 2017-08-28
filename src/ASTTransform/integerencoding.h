/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   integerencoding.h
 * Author: hamed
 *
 * Created on August 24, 2017, 5:31 PM
 */

#ifndef INTEGERENCODING_H
#define INTEGERENCODING_H
#include "classlist.h"
#include "structs.h"

Element *getOrderIntegerElement(CSolver *solver, Order *order, uint64_t item);
void orderIntegerEncodingSATEncoder(CSolver *solver, BooleanOrder *boolOrder);

#endif/* INTEGERENCODING_H */

