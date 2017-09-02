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
	void decomposeOrder (Order *currOrder, OrderGraph *currGraph);

	CMEMALLOC;
private:
	Order *currOrder;
	OrderGraph *currGraph;
};

#endif/* ORDERTRANSFORM_H */

