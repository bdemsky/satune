#include "rewriter.h"
#include "boolean.h"
#include "csolver.h"

void CSolver::replaceBooleanWithTrue(Boolean *bexpr) {
	if (constraints.contains(bexpr)) {
		constraints.remove(bexpr);
	}

	uint size = bexpr->parents.getSize();
	for (uint i = 0; i < size; i++) {
		Boolean *parent = bexpr->parents.get(i);
		BooleanLogic *logicop = (BooleanLogic *) parent;
		switch (logicop->op) {
		case SATC_AND:
			handleANDTrue(logicop, bexpr);
			break;
		case SATC_NOT:
			replaceBooleanWithFalse(parent);
			break;
		case SATC_IFF:
			handleIFFTrue(logicop, bexpr);
			break;
		case SATC_OR:
		case SATC_XOR:
		case SATC_IMPLIES:
			ASSERT(0);
		}
	}
}

void CSolver::replaceBooleanWithBoolean(Boolean *oldb, Boolean *newb) {
	if (constraints.contains(oldb)) {
		constraints.remove(oldb);
		constraints.add(newb);
	}

	uint size = oldb->parents.getSize();
	for (uint i = 0; i < size; i++) {
		Boolean *parent = oldb->parents.get(i);
		BooleanLogic *logicop = (BooleanLogic *) parent;

		uint parentsize = logicop->inputs.getSize();

		for (uint j = 0; j < parentsize; j++) {
			Boolean *b = logicop->inputs.get(i);
			if (b == oldb) {
				logicop->inputs.set(i, newb);
				newb->parents.push(parent);
			}
		}
	}
}

void handleIFFFalse(BooleanLogic *bexpr, Boolean *child) {
	uint size = bexpr->inputs.getSize();
	Boolean *b = bexpr->inputs.get(0);
	uint childindex = (b == child) ? 0 : 1;
	bexpr->inputs.remove(childindex);
	bexpr->op = SATC_NOT;
}

void CSolver::handleIFFTrue(BooleanLogic *bexpr, Boolean *child) {
	uint size = bexpr->inputs.getSize();
	Boolean *b = bexpr->inputs.get(0);
	uint otherindex = (b == child) ? 1 : 0;
	replaceBooleanWithBoolean(bexpr, bexpr->inputs.get(otherindex));
}

void CSolver::handleANDTrue(BooleanLogic *bexpr, Boolean *child) {
	uint size = bexpr->inputs.getSize();

	if (size == 1) {
		replaceBooleanWithTrue(bexpr);
		return;
	}

	for (uint i = 0; i < size; i++) {
		Boolean *b = bexpr->inputs.get(i);
		if (b == child) {
			bexpr->inputs.remove(i);
		}
	}

	if (size == 2) {
		replaceBooleanWithBoolean(bexpr, bexpr->inputs.get(0));
	}
}

void CSolver::replaceBooleanWithFalse(Boolean *bexpr) {
	if (constraints.contains(bexpr)) {
		setUnSAT();
		constraints.remove(bexpr);
	}

	uint size = bexpr->parents.getSize();
	for (uint i = 0; i < size; i++) {
		Boolean *parent = bexpr->parents.get(i);
		BooleanLogic *logicop = (BooleanLogic *) parent;
		switch (logicop->op) {
		case SATC_AND:
			replaceBooleanWithFalse(parent);
			break;
		case SATC_NOT:
			replaceBooleanWithTrue(parent);
			break;
		case SATC_IFF:
			handleIFFFalse(logicop, bexpr);
			break;
		case SATC_OR:
		case SATC_XOR:
		case SATC_IMPLIES:
			ASSERT(0);
		}
	}
}
