#include "iterator.h"
#include "boolean.h"
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
				boolean.push(logicop->inputs.get(topindex).getBoolean());
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
