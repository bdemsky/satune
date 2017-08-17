#include "rewriter.h"
#include "boolean.h"

void replaceBooleanWithTrue(Boolean *This) {
	uint size = getSizeVectorBoolean(&This->parents);
	for (uint i = 0; i < size; i++) {
		Boolean *parent = getVectorBoolean(&This->parents, i);
		BooleanLogic *logicop = (BooleanLogic *) parent;
		switch (logicop->op) {
		case L_AND:
			handleANDTrue(logicop, This);
			break;
		case L_OR:
			replaceBooleanWithTrue(parent);
			break;
		case L_NOT:
			replaceBooleanWithFalse(parent);
			break;
		case L_XOR:
			handleXORTrue(logicop, This);
			break;
		case L_IMPLIES:
			handleIMPLIESTrue(logicop, This);
			break;
		}
	}
}

void replaceBooleanWithBoolean(Boolean *oldb, Boolean *newb) {
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

void handleXORTrue(BooleanLogic *This, Boolean *child) {
	uint size = getSizeArrayBoolean(&This->inputs);
	Boolean *b = getArrayBoolean(&This->inputs, 0);
	uint otherindex = (b == child) ? 1 : 0;
	removeElementArrayBoolean(&This->inputs, otherindex);
	This->op = L_NOT;
}

void handleXORFalse(BooleanLogic *This, Boolean *child) {
	uint size = getSizeArrayBoolean(&This->inputs);
	Boolean *b = getArrayBoolean(&This->inputs, 0);
	uint otherindex = (b == child) ? 1 : 0;
	replaceBooleanWithBoolean((Boolean *)This, getArrayBoolean(&This->inputs, otherindex));
}

void handleIMPLIESTrue(BooleanLogic *This, Boolean *child) {
	uint size = getSizeArrayBoolean(&This->inputs);
	Boolean *b = getArrayBoolean(&This->inputs, 0);
	if (b == child) {
		//Replace with other term
		replaceBooleanWithBoolean((Boolean *)This, getArrayBoolean(&This->inputs, 1));
	} else {
		//Statement is true...
		replaceBooleanWithTrue((Boolean *)This);
	}
}

void handleIMPLIESFalse(BooleanLogic *This, Boolean *child) {
	uint size = getSizeArrayBoolean(&This->inputs);
	Boolean *b = getArrayBoolean(&This->inputs, 0);
	if (b == child) {
		//Statement is true...
		replaceBooleanWithTrue((Boolean *)This);
	} else {
		//Make into negation of first term
		removeElementArrayBoolean(&This->inputs, 1);
		This->op = L_NOT;
	}
}

void handleANDTrue(BooleanLogic *This, Boolean *child) {
	uint size = getSizeArrayBoolean(&This->inputs);

	if (size == 1) {
		replaceBooleanWithTrue((Boolean *)This);
		return;
	}

	for (uint i = 0; i < size; i++) {
		Boolean *b = getArrayBoolean(&This->inputs, i);
		if (b == child) {
			removeElementArrayBoolean(&This->inputs, i);
		}
	}

	if (size == 2) {
		replaceBooleanWithBoolean((Boolean *)This, getArrayBoolean(&This->inputs, 0));
	}
}

void handleORFalse(BooleanLogic *This, Boolean *child) {
	uint size = getSizeArrayBoolean(&This->inputs);

	if (size == 1) {
		replaceBooleanWithFalse((Boolean *) This);
	}

	for (uint i = 0; i < size; i++) {
		Boolean *b = getArrayBoolean(&This->inputs, i);
		if (b == child) {
			removeElementArrayBoolean(&This->inputs, i);
		}
	}

	if (size == 2) {
		replaceBooleanWithBoolean((Boolean *)This, getArrayBoolean(&This->inputs, 0));
	}
}

void replaceBooleanWithFalse(Boolean *This) {
	uint size = getSizeVectorBoolean(&This->parents);
	for (uint i = 0; i < size; i++) {
		Boolean *parent = getVectorBoolean(&This->parents, i);
		BooleanLogic *logicop = (BooleanLogic *) parent;
		switch (logicop->op) {
		case L_AND:
			replaceBooleanWithFalse(parent);
			break;
		case L_OR:
			handleORFalse(logicop, This);
			break;
		case L_NOT:
			replaceBooleanWithTrue(parent);
			break;
		case L_XOR:
			handleXORFalse(logicop, This);
			break;
		case L_IMPLIES:
			handleIMPLIESFalse(logicop, This);
			break;
		}
	}
}
