/*
 * File:   orderedge.h
 * Author: hamed
 *
 * Created on August 7, 2017, 3:44 PM
 */

#ifndef ORDEREDGE_H
#define ORDEREDGE_H
#include "classlist.h"
#include "mymemory.h"

struct OrderEdge {
	OrderNode *source;
	OrderNode *sink;
	unsigned int polPos : 1;
	unsigned int polNeg : 1;
	unsigned int mustPos : 1;
	unsigned int mustNeg : 1;
	unsigned int pseudoPos : 1;
};

OrderEdge *allocOrderEdge(OrderNode *begin, OrderNode *end);
void deleteOrderEdge(OrderEdge *This);
void setPseudoPos(OrderGraph *graph, OrderEdge *edge);
void setMustPos(OrderGraph *graph, OrderEdge *edge);
void setMustNeg(OrderGraph *graph, OrderEdge *edge);
void setPolPos(OrderGraph *graph, OrderEdge *edge);
void setPolNeg(OrderGraph *graph, OrderEdge *edge);

#endif/* ORDEREDGE_H */

