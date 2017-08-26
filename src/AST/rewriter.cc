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
		case L_AND:
			handleANDTrue(logicop, bexpr);
			break;
		case L_OR:
			replaceBooleanWithTrue(parent);
			break;
		case L_NOT:
			replaceBooleanWithFalse(parent);
			break;
		case L_XOR:
			handleXORTrue(logicop, bexpr);
			break;
		case L_IMPLIES:
			handleIMPLIESTrue(logicop, bexpr);
			break;
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

void handleXORTrue(BooleanLogic *bexpr, Boolean *child) {
	uint size = bexpr->inputs.getSize();
	Boolean *b = bexpr->inputs.get(0);
	uint childindex = (b == child) ? 0 : 1;
	bexpr->inputs.remove(childindex);
	bexpr->op = L_NOT;
}

void CSolver::handleXORFalse(BooleanLogic *bexpr, Boolean *child) {
	uint size = bexpr->inputs.getSize();
	Boolean *b = bexpr->inputs.get(0);
	uint otherindex = (b == child) ? 1 : 0;
	replaceBooleanWithBoolean(bexpr, bexpr->inputs.get(otherindex));
}

void CSolver::handleIMPLIESTrue(BooleanLogic *bexpr, Boolean *child) {
	uint size = bexpr->inputs.getSize();
	Boolean *b = bexpr->inputs.get(0);
	if (b == child) {
		//Replace with other term
		replaceBooleanWithBoolean(bexpr, bexpr->inputs.get(1));
	} else {
		//Statement is true...
		replaceBooleanWithTrue(bexpr);
	}
}

void CSolver::handleIMPLIESFalse(BooleanLogic *bexpr, Boolean *child) {
	uint size = bexpr->inputs.getSize();
	Boolean *b = bexpr->inputs.get(0);
	if (b == child) {
		//Statement is true...
		replaceBooleanWithTrue(bexpr);
	} else {
		//Make into negation of first term
		bexpr->inputs.get(1);
		bexpr->op = L_NOT;
	}
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

void CSolver::handleORFalse(BooleanLogic *bexpr, Boolean *child) {
	uint size = bexpr->inputs.getSize();

	if (size == 1) {
		replaceBooleanWithFalse(bexpr);
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
		case L_AND:
			replaceBooleanWithFalse(parent);
			break;
		case L_OR:
			handleORFalse(logicop, bexpr);
			break;
		case L_NOT:
			replaceBooleanWithTrue(parent);
			break;
		case L_XOR:
			handleXORFalse(logicop, bexpr);
			break;
		case L_IMPLIES:
			handleIMPLIESFalse(logicop, bexpr);
			break;
		}
	}
}
