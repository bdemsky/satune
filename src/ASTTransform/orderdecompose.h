/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   orderdecompose.h
 * Author: hamed
 *
 * Created on August 24, 2017, 5:33 PM
 */

#ifndef ORDERDECOMPOSE_H
#define ORDERDECOMPOSE_H
#include "classlist.h"
#include "structs.h"

void orderAnalysis(CSolver *This);
void decomposeOrder(CSolver *This, Order *order, OrderGraph *graph);

#endif /* ORDERDECOMPOSE_H */

