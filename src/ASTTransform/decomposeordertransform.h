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
	DecomposeOrderTransform(CSolver* _solver, Order* order, Tunables _tunable, TunableDesc* _desc);
	virtual ~DecomposeOrderTransform();
	void doTransform();
	void setOrderGraph(OrderGraph* _graph){
		graph = _graph;
	}
	bool canExecuteTransform();
private:
	Order* order;
	OrderGraph* graph;
};

#endif /* ORDERTRANSFORM_H */

