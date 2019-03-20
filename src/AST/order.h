#ifndef ORDER_H
#define ORDER_H
#include "classlist.h"
#include "mymemory.h"
#include "structs.h"
#include "ops.h"
#include "orderencoding.h"
#include "boolean.h"
#include "orderpair.h"

class Order {
public:
	Order(OrderType type, Set *set);
	virtual ~Order();
	OrderType type;
	Set *set;
	OrderGraph *graph;
	OrderEncoding encoding;
	Order *clone(CSolver *solver, CloneMap *map);
	void serialize(Serializer *serializer );
	void print();
	void setOrderResolver(OrderResolver *_resolver) { ASSERT(encoding.resolver == NULL); encoding.resolver = _resolver;}
	void initializeOrderElementsHashtable();
	void addOrderConstraint(BooleanOrder *constraint);
	void setOrderEncodingType(OrderEncodingType type);
	HashtableOrderPair *getOrderPairTable();
	CMEMALLOC;
private:
	Hashset64Int useditems;
	Vector<BooleanOrder *> constraints;
public:
	Vector<BooleanOrder *> *getConstraints() {return &constraints;}
	uint getNumUsed() {return constraints.getSize();}
	SetIterator64Int *getUsedIterator() {return useditems.iterator();}

};

#endif
