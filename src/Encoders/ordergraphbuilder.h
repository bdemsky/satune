/* 
 * File:   ordergraphbuilder.h
 * Author: hamed
 *
 * Created on August 8, 2017, 6:36 PM
 */

#ifndef ORDERGRAPHBUILDER_H
#define ORDERGRAPHBUILDER_H
#include "classlist.h"

void buildOrderGraph(CSolver* This);
void computeStronglyConnectedComponentGraph(OrderGraph* graph);
#endif /* ORDERGRAPHBUILDER_H */

