#include "asthash.h"
#include "mymemory.h"
#include "structs.h"
#include "boolean.h"
#include "element.h"

bool compareArray(Array<BooleanEdge> *inputs1, Array<BooleanEdge> *inputs2) {
	if (inputs1->getSize() != inputs2->getSize())
		return false;
	for (uint i = 0; i < inputs1->getSize(); i++) {
		if (inputs1->get(i) != inputs2->get(i))
			return false;
	}
	return true;
}

bool compareArray(Array<Element *> *inputs1, Array<Element *> *inputs2) {
	if (inputs1->getSize() != inputs2->getSize())
		return false;
	for (uint i = 0; i < inputs1->getSize(); i++) {
		if (inputs1->get(i) != inputs2->get(i))
			return false;
	}
	return true;
}

uint hashArray(Array<BooleanEdge> *inputs) {
	uint hash = inputs->getSize();
	for (uint i = 0; i < inputs->getSize(); i++) {
		uint newval = (uint)(uintptr_t) inputs->get(i).getRaw();
		hash ^= newval;
		hash = (hash << 3) | (hash >> 29);
	}
	return hash;
}

uint hashArray(Array<Element *> *inputs) {
	uint hash = inputs->getSize();
	for (uint i = 0; i < inputs->getSize(); i++) {
		uint newval = (uint)(uintptr_t) inputs->get(i);
		hash ^= newval;
		hash = (hash << 3) | (hash >> 29);
	}
	return hash;
}

uint hashBoolean(Boolean *b) {
	switch (b->type) {
	case ORDERCONST: {
		BooleanOrder *o = (BooleanOrder *)b;
		return ((uint)(uintptr_t) o->order) ^ ((uint) o->first) ^ (((uint)(o->second)) << 2);
	}
	case BOOLEANVAR: {
		return (uint)(uintptr_t) b;
	}
	case LOGICOP: {
		BooleanLogic *l = (BooleanLogic *)b;
		return ((uint)l->op) ^ hashArray(&l->inputs);
	}
	case PREDICATEOP: {
		BooleanPredicate *p = (BooleanPredicate *)b;
		return ((uint)(uintptr_t) p->predicate) ^
					 (((uint)(uintptr_t) p->undefStatus) << 1) ^
					 hashArray(&p->inputs);
	}
	default:
		ASSERT(0);
	}
}

bool compareBoolean(Boolean *b1, Boolean *b2) {
	if (b1->type != b2->type)
		return false;
	switch (b1->type) {
	case ORDERCONST: {
		BooleanOrder *o1 = (BooleanOrder *)b1;
		BooleanOrder *o2 = (BooleanOrder *)b2;
		return (o1->order == o2->order) && (o1->first == o2->first) && (o1->second == o2->second);
	}
	case BOOLEANVAR: {
		return b1 == b2;
	}
	case LOGICOP: {
		BooleanLogic *l1 = (BooleanLogic *)b1;
		BooleanLogic *l2 = (BooleanLogic *)b2;
		return (l1->op == l2->op) && compareArray(&l1->inputs, &l2->inputs);
	}
	case PREDICATEOP: {
		BooleanPredicate *p1 = (BooleanPredicate *)b1;
		BooleanPredicate *p2 = (BooleanPredicate *)b2;
		return (p1->predicate == p2->predicate) &&
					 p1->undefStatus == p2->undefStatus &&
					 compareArray(&p1->inputs, &p2->inputs);
	}
	default:
		ASSERT(0);
	}
}

uint hashElement(Element *e) {
	switch (e->type) {
	case ELEMSET: {
		return (uint)(uintptr_t) e;
	}
	case ELEMFUNCRETURN: {
		ElementFunction *ef = (ElementFunction *) e;
		return ((uint)(uintptr_t) ef->function) ^
					 ((uint)(uintptr_t) ef->overflowstatus) ^
					 hashArray(&ef->inputs);
	}
	case ELEMCONST: {
		ElementConst *ec = (ElementConst *) e;
		return ((uint) ec->value);
	}
	default:
		ASSERT(0);
	}
}

bool compareElement(Element *e1, Element *e2) {
	if (e1->type != e2->type)
		return false;
	switch (e1->type) {
	case ELEMSET: {
		return e1 == e2;
	}
	case ELEMFUNCRETURN: {
		ElementFunction *ef1 = (ElementFunction *) e1;
		ElementFunction *ef2 = (ElementFunction *) e2;
		return (ef1->function == ef2->function) &&
					 (ef1->overflowstatus == ef2->overflowstatus) &&
					 compareArray(&ef1->inputs, &ef2->inputs);
	}
	case ELEMCONST: {
		ElementConst *ec1 = (ElementConst *) e1;
		ElementConst *ec2 = (ElementConst *) e2;
		return (ec1->value == ec2->value);
	}
	default:
		ASSERT(0);
	}
}
