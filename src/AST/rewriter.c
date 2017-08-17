#include "rewriter.h"
#include "boolean.h"

void replaceBooleanWithTrue(CSolver * This, Boolean *bexpr) {
	uint size = getSizeVectorBoolean(&bexpr->parents);
	for (uint i = 0; i < size; i++) {
		Boolean *parent = getVectorBoolean(&bexpr->parents, i);
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
	uint size = getSizeVectorBoolean(&oldb->parents);
	for (uint i = 0; i < size; i++) {
		Boolean *parent = getVectorBoolean(&oldb->parents, i);
		BooleanLogic *logicop = (BooleanLogic *) parent;

		uint parentsize = getSizeArrayBoolean(&logicop->inputs);

		for (uint j = 0; j < parentsize; j++) {
			Boolean *b = getArrayBoolean(&logicop->inputs, i);
			if (b == oldb) {
				setArrayBoolean(&logicop->inputs, i, newb);
				pushVectorBoolean(&newb->parents, parent);
			}
		}
	}
}

void handleXORTrue(BooleanLogic *bexpr, Boolean *child) {
	uint size = getSizeArrayBoolean(&bexpr->inputs);
	Boolean *b = getArrayBoolean(&bexpr->inputs, 0);
	uint otherindex = (b == child) ? 1 : 0;
	removeElementArrayBoolean(&bexpr->inputs, otherindex);
	bexpr->op = L_NOT;
}

void handleXORFalse(CSolver * This, BooleanLogic *bexpr, Boolean *child) {
	uint size = getSizeArrayBoolean(&bexpr->inputs);
	Boolean *b = getArrayBoolean(&bexpr->inputs, 0);
	uint otherindex = (b == child) ? 1 : 0;
	replaceBooleanWithBoolean(This, (Boolean *)bexpr, getArrayBoolean(&bexpr->inputs, otherindex));
}

void handleIMPLIESTrue(CSolver * This, BooleanLogic *bexpr, Boolean *child) {
	uint size = getSizeArrayBoolean(&bexpr->inputs);
	Boolean *b = getArrayBoolean(&bexpr->inputs, 0);
	if (b == child) {
		//Replace with other term
		replaceBooleanWithBoolean(This, (Boolean *)bexpr, getArrayBoolean(&bexpr->inputs, 1));
	} else {
		//Statement is true...
		replaceBooleanWithTrue(This, (Boolean *)bexpr);
	}
}

void handleIMPLIESFalse(CSolver * This, BooleanLogic *bexpr, Boolean *child) {
	uint size = getSizeArrayBoolean(&bexpr->inputs);
	Boolean *b = getArrayBoolean(&bexpr->inputs, 0);
	if (b == child) {
		//Statement is true...
		replaceBooleanWithTrue(This, (Boolean *)bexpr);
	} else {
		//Make into negation of first term
		removeElementArrayBoolean(&bexpr->inputs, 1);
		bexpr->op = L_NOT;
	}
}

void handleANDTrue(CSolver * This, BooleanLogic *bexpr, Boolean *child) {
	uint size = getSizeArrayBoolean(&bexpr->inputs);

	if (size == 1) {
		replaceBooleanWithTrue(This, (Boolean *)bexpr);
		return;
	}

	for (uint i = 0; i < size; i++) {
		Boolean *b = getArrayBoolean(&bexpr->inputs, i);
		if (b == child) {
			removeElementArrayBoolean(&bexpr->inputs, i);
		}
	}

	if (size == 2) {
		replaceBooleanWithBoolean(This, (Boolean *)bexpr, getArrayBoolean(&bexpr->inputs, 0));
	}
}

void handleORFalse(CSolver * This, BooleanLogic *bexpr, Boolean *child) {
	uint size = getSizeArrayBoolean(&bexpr->inputs);

	if (size == 1) {
		replaceBooleanWithFalse(This, (Boolean *) bexpr);
	}

	for (uint i = 0; i < size; i++) {
		Boolean *b = getArrayBoolean(&bexpr->inputs, i);
		if (b == child) {
			removeElementArrayBoolean(&bexpr->inputs, i);
		}
	}

	if (size == 2) {
		replaceBooleanWithBoolean(This, (Boolean *)bexpr, getArrayBoolean(&bexpr->inputs, 0));
	}
}

void replaceBooleanWithFalse(CSolver * This, Boolean *bexpr) {
	uint size = getSizeVectorBoolean(&bexpr->parents);
	for (uint i = 0; i < size; i++) {
		Boolean *parent = getVectorBoolean(&bexpr->parents, i);
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
