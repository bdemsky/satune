#include "rewriter.h"
#include "boolean.h"
#include "csolver.h"

void replaceBooleanWithTrue(CSolver * This, Boolean *bexpr) {
	if (containsHashSetBoolean(This->constraints, bexpr)) {
		removeHashSetBoolean(This->constraints, bexpr);
	}

	uint size = bexpr->parents.getSize();
	for (uint i = 0; i < size; i++) {
		Boolean *parent = bexpr->parents.get(i);
		BooleanLogic *logicop = (BooleanLogic *) parent;
		switch (logicop->op) {
		case L_AND:
			handleANDTrue(This, logicop, bexpr);
			break;
		case L_OR:
			replaceBooleanWithTrue(This, parent);
			break;
		case L_NOT:
			replaceBooleanWithFalse(This, parent);
			break;
		case L_XOR:
			handleXORTrue(logicop, bexpr);
			break;
		case L_IMPLIES:
			handleIMPLIESTrue(This, logicop, bexpr);
			break;
		}
	}
}

void replaceBooleanWithBoolean(CSolver * This, Boolean *oldb, Boolean *newb) {
	if (containsHashSetBoolean(This->constraints, oldb)) {
		removeHashSetBoolean(This->constraints, oldb);
		addHashSetBoolean(This->constraints, newb);
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

void handleXORFalse(CSolver * This, BooleanLogic *bexpr, Boolean *child) {
	uint size = bexpr->inputs.getSize();
	Boolean *b = bexpr->inputs.get(0);
	uint otherindex = (b == child) ? 1 : 0;
	replaceBooleanWithBoolean(This, (Boolean *)bexpr, bexpr->inputs.get(otherindex));
}

void handleIMPLIESTrue(CSolver * This, BooleanLogic *bexpr, Boolean *child) {
	uint size = bexpr->inputs.getSize();
	Boolean *b = bexpr->inputs.get(0);
	if (b == child) {
		//Replace with other term
		replaceBooleanWithBoolean(This, (Boolean *)bexpr, bexpr->inputs.get(1));
	} else {
		//Statement is true...
		replaceBooleanWithTrue(This, (Boolean *)bexpr);
	}
}

void handleIMPLIESFalse(CSolver * This, BooleanLogic *bexpr, Boolean *child) {
	uint size = bexpr->inputs.getSize();
	Boolean *b = bexpr->inputs.get(0);
	if (b == child) {
		//Statement is true...
		replaceBooleanWithTrue(This, (Boolean *)bexpr);
	} else {
		//Make into negation of first term
		bexpr->inputs.get(1);
		bexpr->op = L_NOT;
	}
}

void handleANDTrue(CSolver * This, BooleanLogic *bexpr, Boolean *child) {
	uint size = bexpr->inputs.getSize();

	if (size == 1) {
		replaceBooleanWithTrue(This, (Boolean *)bexpr);
		return;
	}

	for (uint i = 0; i < size; i++) {
		Boolean *b = bexpr->inputs.get(i);
		if (b == child) {
			bexpr->inputs.remove(i);
		}
	}

	if (size == 2) {
		replaceBooleanWithBoolean(This, (Boolean *)bexpr, bexpr->inputs.get(0));
	}
}

void handleORFalse(CSolver * This, BooleanLogic *bexpr, Boolean *child) {
	uint size = bexpr->inputs.getSize();

	if (size == 1) {
		replaceBooleanWithFalse(This, (Boolean *) bexpr);
	}

	for (uint i = 0; i < size; i++) {
		Boolean *b = bexpr->inputs.get(i);
		if (b == child) {
			bexpr->inputs.remove(i);
		}
	}

	if (size == 2) {
		replaceBooleanWithBoolean(This, (Boolean *)bexpr, bexpr->inputs.get(0));
	}
}

void replaceBooleanWithFalse(CSolver * This, Boolean *bexpr) {
	if (containsHashSetBoolean(This->constraints, bexpr)) {
		This->unsat=true;
		removeHashSetBoolean(This->constraints, bexpr);
	}
	
	uint size = bexpr->parents.getSize();
	for (uint i = 0; i < size; i++) {
		Boolean *parent = bexpr->parents.get(i);
		BooleanLogic *logicop = (BooleanLogic *) parent;
		switch (logicop->op) {
		case L_AND:
			replaceBooleanWithFalse(This, parent);
			break;
		case L_OR:
			handleORFalse(This, logicop, bexpr);
			break;
		case L_NOT:
			replaceBooleanWithTrue(This, parent);
			break;
		case L_XOR:
			handleXORFalse(This, logicop, bexpr);
			break;
		case L_IMPLIES:
			handleIMPLIESFalse(This, logicop, bexpr);
			break;
		}
	}
}
