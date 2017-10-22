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
	void bypassMustBeTrueNode(OrderGraph *graph, OrderNode *node, DecomposeOrderResolver *dor);
	void decomposeOrder(Order *currOrder, OrderGraph *currGraph, DecomposeOrderResolver *dor);
	void removeMustBeTrueNodes(OrderGraph *graph, DecomposeOrderResolver *dor);
	void mustEdgePrune(OrderGraph *graph, DecomposeOrderResolver *dor);
	void attemptNodeMerge(OrderGraph *graph, OrderNode *node, DecomposeOrderResolver *dor);
	void mergeNodes(OrderGraph *graph, OrderNode *node, OrderEdge *edge, OrderNode *dstnode, DecomposeOrderResolver *dor);
};



#endif/* ORDERTRANSFORM_H */

