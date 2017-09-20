#include "iterator.h"
#include "boolean.h"
#include "element.h"
#include "csolver.h"

BooleanIterator::BooleanIterator(CSolver * _solver) :
	solverit(_solver->getConstraints()) {
	updateNext();
}

BooleanIterator::~BooleanIterator() {
	delete solverit;
}

bool BooleanIterator::hasNext() {
	return boolean.getSize() != 0;
}

void BooleanIterator::updateNext() {
	if (boolean.getSize() != 0) {
		boolean.pop();
		index.pop();
	}
	
	while(true) {
		if (boolean.getSize() == 0) {
			if (solverit->hasNext()) {
				Boolean *b=solverit->next().getBoolean();
				if (discovered.add(b)) {
					boolean.push(b);
					index.push(0);
				} else
					continue;
			} else
				return;
		}
		Boolean *topboolean=boolean.last();
		uint topindex=index.last();
		switch(topboolean->type) {
		case ORDERCONST:
		case BOOLEANVAR:
		case PREDICATEOP:
		case BOOLCONST:
			return;
		case LOGICOP: {
			BooleanLogic * logicop=(BooleanLogic*) topboolean;
			uint size=logicop->inputs.getSize();
			if (topindex == size)
				return;
			index.pop();
			index.push(topindex+1);
			Boolean *newchild=logicop->inputs.get(topindex).getBoolean();
			if (discovered.add(newchild)) {
				boolean.push(newchild);
				index.push(0);
			}
			break;
		}
		default:
			ASSERT(0);
		}
	}
}

Boolean * BooleanIterator::next() {
	Boolean * b = boolean.last();
	updateNext();
	return b;
}

ElementIterator::ElementIterator(CSolver *_solver) :
	bit(_solver),
	base(NULL),
	baseindex(0) {
	updateNext();
}

ElementIterator::~ElementIterator() {
}

bool ElementIterator::hasNext() {
	return element.getSize() != 0;
}

void ElementIterator::updateNext() {
	if (element.getSize() != 0) {
		element.pop();
		index.pop();
	}
	
	while(true) {
		if (element.getSize() == 0) {
			if (base != NULL) {
				if (baseindex == base->inputs.getSize()) {
					base = NULL;
					continue;
				} else {
					Element * e = base->inputs.get(baseindex);
					baseindex++;
					if (discovered.add(e)) {
						element.push(e);
						index.push(0);
					} else
						continue;
				}
			} else {
				if (bit.hasNext()) {
					Boolean *b=bit.next();
					if (b->type == PREDICATEOP) {
						base = (BooleanPredicate *)b;
						baseindex=0;
					}
					continue;
				} else
					return;
			}
		}
		Element *topelement=element.last();
		uint topindex=index.last();
		switch(topelement->type) {
		case ELEMSET:
		case ELEMCONST:
			return;
		case ELEMFUNCRETURN: {
			ElementFunction * func=(ElementFunction*) topelement;
			uint size=func->inputs.getSize();
			if (topindex == size)
				return;
			index.pop();
			index.push(topindex+1);
			Element *newchild=func->inputs.get(topindex);
			if (discovered.add(newchild)) {
				element.push(newchild);
				index.push(0);
			}
			break;
		}
		default:
			ASSERT(0);
		}
	}
}

Element * ElementIterator::next() {
	Element * e = element.last();
	updateNext();
	return e;
}
