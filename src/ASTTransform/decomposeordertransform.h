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
	DecomposeOrderTransform(CSolver* _solver);
	virtual ~DecomposeOrderTransform();
	void doTransform();
	void setOrderGraph(OrderGraph* _graph){
		currGraph = _graph;
	}
	void setCurrentOrder(Order* _current) { currOrder = _current;}
	bool canExecuteTransform();
private:
	Order* currOrder;
	OrderGraph* currGraph;
};

#endif /* ORDERTRANSFORM_H */

