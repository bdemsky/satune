#include "rewriter.h"
#include "boolean.h"
#include "csolver.h"
#include "polarityassignment.h"

void CSolver::replaceBooleanWithTrue(BooleanEdge bexpr) {
	if (constraints.contains(bexpr.negate())) {
		constraints.remove(bexpr.negate());
		setUnSAT();
	}	
	if (constraints.contains(bexpr)) {
		constraints.remove(bexpr);
	}

	replaceBooleanWithTrueNoRemove(bexpr);
}
	
void CSolver::replaceBooleanWithTrueNoRemove(BooleanEdge bexpr) {
	updateMustValue(bexpr.getBoolean(), bexpr.isNegated() ? BV_MUSTBEFALSE : BV_MUSTBETRUE);
	
	uint size = bexpr->parents.getSize();
	for (uint i = 0; i < size; i++) {
		Boolean *parent = bexpr->parents.get(i);
		ASSERT(parent->type == LOGICOP);
		BooleanLogic *logicop = (BooleanLogic *) parent;
		switch (logicop->op) {
		case SATC_AND:
			handleANDTrue(logicop, bexpr);
			break;
		case SATC_IFF:
			handleIFFTrue(logicop, bexpr);
			break;
		case SATC_NOT:
		case SATC_OR:
		case SATC_XOR:
		case SATC_IMPLIES:
			ASSERT(0);
		}
	}
}

void CSolver::replaceBooleanWithBoolean(BooleanEdge oldb, BooleanEdge newb) {
	//Canonicalize
	if (oldb.isNegated()) {
		oldb=oldb.negate();
		newb=newb.negate();
	}
	if (constraints.contains(oldb)) {
		constraints.remove(oldb);
		constraints.add(newb);
	}
	if (constraints.contains(oldb.negate())) {
		constraints.remove(oldb.negate());
		constraints.add(newb.negate());
	}

	BooleanEdge oldbnegated = oldb.negate();
	uint size = oldb->parents.getSize();
	for (uint i = 0; i < size; i++) {
		Boolean *parent = oldb->parents.get(i);
		BooleanLogic *logicop = (BooleanLogic *) parent;

		uint parentsize = logicop->inputs.getSize();

		for (uint j = 0; j < parentsize; j++) {
			BooleanEdge b = logicop->inputs.get(i);
			if (b == oldb) {
				logicop->inputs.set(i, newb);
				newb->parents.push(parent);
			} else if (b == oldbnegated) {
				logicop->inputs.set(i, newb.negate());
				newb->parents.push(parent);
			}
		}
	}
}

void CSolver::handleIFFTrue(BooleanLogic *bexpr, BooleanEdge child) {
	uint size = bexpr->inputs.getSize();
	BooleanEdge b0 = bexpr->inputs.get(0);
	BooleanEdge b1 = bexpr->inputs.get(1);
	BooleanEdge childnegate = child.negate();
	bexpr->replaced = true;
	if (b0 == child) {
		replaceBooleanWithBoolean(BooleanEdge(bexpr), b1);
	} else if (b0 == childnegate) {
		replaceBooleanWithBoolean(BooleanEdge(bexpr), b1.negate());
	} else if (b1 == child) {
		replaceBooleanWithBoolean(BooleanEdge(bexpr), b0);
	} else if (b1 == childnegate) {
		replaceBooleanWithBoolean(BooleanEdge(bexpr), b0.negate());
	} else
		ASSERT(0);
}

void CSolver::handleANDTrue(BooleanLogic *bexpr, BooleanEdge child) {
	BooleanEdge childNegate=child.negate();
	
	boolMap.remove(bexpr);
	
	for (uint i = 0; i < bexpr->inputs.getSize(); i++) {
		BooleanEdge b = bexpr->inputs.get(i);
		
		if (b == child) {
			bexpr->inputs.remove(i);
			i--;
		} else if (b == childNegate) {
			replaceBooleanWithFalse(bexpr);
			return;
		}
	}

	uint size=bexpr->inputs.getSize();
	if (size == 0) {
		bexpr->replaced = true;
		replaceBooleanWithTrue(bexpr);
	} else if (size == 1) {
		bexpr->replaced = true;
		replaceBooleanWithBoolean(bexpr, bexpr->inputs.get(0));
	} else {
		//Won't build any of these in future cases...
		boolMap.put(bexpr, bexpr);
	}
}

void CSolver::replaceBooleanWithFalse(BooleanEdge bexpr) {
	replaceBooleanWithTrue(bexpr.negate());
}

BooleanEdge CSolver::doRewrite(BooleanEdge bexpr) {
	bool isNegated=bexpr.isNegated();
	BooleanLogic * b = (BooleanLogic *) bexpr.getBoolean();
	BooleanEdge result;
	if (b->op == SATC_IFF) {
		if (isTrue(b->inputs.get(0))) {
			result = b->inputs.get(1);
		} else if (isFalse(b->inputs.get(0))) {
			result = b->inputs.get(1).negate();
		} else if (isTrue(b->inputs.get(1))) {
			result = b->inputs.get(0);
		} else if (isFalse(b->inputs.get(1))) {
			result = b->inputs.get(0).negate();
		} else ASSERT(0);
	} else if (b->op==SATC_AND) {
		uint size = b->inputs.getSize();
		if (size == 0)
			result = boolTrue;
		else if (size == 1)
			result = b->inputs.get(0);
		else ASSERT(0);
	}
	return isNegated ? result.negate() : result;
}
