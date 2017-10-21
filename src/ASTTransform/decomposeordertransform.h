/*
 * File:   ordertransform.h
 * Author: hamed
 *
 * Created on August 28, 2017, 10:35 AM
 */

#ifndef ORDERTRANSFORM_H
#define ORDERTRANSFORM_H
#include "classlist.h"
#include "transform.h"


class DecomposeOrderTransform : public Transform {
public:
	DecomposeOrderTransform(CSolver *_solver);
	~DecomposeOrderTransform();
	void doTransform();

	CMEMALLOC;
private:
	bool isMustBeTrueNode(OrderNode *node);
	void bypassMustBeTrueNode(OrderGraph *graph, OrderNode *node, HashsetOrderEdge *edgesRemoved);
	void decomposeOrder(Order *currOrder, OrderGraph *currGraph, HashsetOrderEdge *edgesRemoved, DecomposeOrderResolver *dor);
	void removeMustBeTrueNodes(OrderGraph *graph, HashsetOrderEdge *edgesRemoved);
};



#endif/* ORDERTRANSFORM_H */

