
/* 
 * File:   orderresolver.h
 * Author: hamed
 *
 * Created on August 31, 2017, 7:16 PM
 */

#ifndef ORDERRESOLVER_H
#define ORDERRESOLVER_H
#include "classlist.h"
#include "structs.h"
#include "mymemory.h"

class OrderResolver {
public:
	OrderResolver(OrderGraph* _graph);
	HappenedBefore resolveOrder(uint64_t first, uint64_t second);
	virtual ~OrderResolver();
	CMEMALLOC;
protected:
	OrderGraph* graph;
	virtual HappenedBefore getOrder(OrderNode* from, OrderNode* to) = 0;
};

#endif /* ORDERRESOLVER_H */

