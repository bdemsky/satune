#include "elementopt.h"
#include "csolver.h"
#include "tunable.h"
#include "iterator.h"
#include "boolean.h"
#include "element.h"
#include "predicate.h"
#include "set.h"

ElementOpt::ElementOpt(CSolver *_solver)
	: Transform(_solver),
	updateSets(false)
{
}

ElementOpt::~ElementOpt() {
}

void ElementOpt::doTransform() {
	if (solver->isUnSAT() || solver->getTuner()->getTunable(ELEMENTOPT, &onoff) == 0)
		return;

	//Set once we know we are going to use it.
	updateSets = solver->getTuner()->getTunable(ELEMENTOPTSETS, &onoff) == 1;

	SetIteratorBooleanEdge *iterator = solver->getConstraints();
	while (iterator->hasNext()) {
		BooleanEdge constraint = iterator->next();
		if (!solver->isConstraintEncoded(constraint) && constraint->type == PREDICATEOP)
			workList.push((BooleanPredicate *)constraint.getBoolean());
	}
	while (workList.getSize() != 0) {
		BooleanPredicate *pred = workList.last(); workList.pop();
		processPredicate(pred);
	}
	delete iterator;
}

void ElementOpt::processPredicate(BooleanPredicate *pred) {
	uint numInputs = pred->inputs.getSize();
	if (numInputs != 2)
		return;

	Predicate *p = pred->getPredicate();
	if (p->type == TABLEPRED)
		return;

	PredicateOperator *pop = (PredicateOperator *) p;
	CompOp op = pop->getOp();

	Element *left = pred->inputs.get(0);
	Element *right = pred->inputs.get(1);

	if (left->type == ELEMCONST) {
		Element *tmp = left;
		left = right;
		right = tmp;
		op = flipOp(op);
	} else if (right->type != ELEMCONST)
		return;

	if (left->type != ELEMSET)
		return;

	if (op == SATC_EQUALS) {
		handlePredicateEquals(pred, (ElementSet *) left, (ElementConst *) right);
	} else if (updateSets) {
		handlePredicateInequality(pred, (ElementSet *) left, (ElementConst *) right);
	}
}

void ElementOpt::handlePredicateEquals(BooleanPredicate *pred, ElementSet *left, ElementConst *right) {
	if (pred->isTrue()) {
		replaceVarWithConst(pred, left, right);
	} else if (pred->isFalse() && updateSets) {
		constrainVarWithConst(pred, left, right);
	}
}

void ElementOpt::handlePredicateInequality(BooleanPredicate *pred, ElementSet *var, ElementConst *value) {
	Predicate *p = pred->getPredicate();
	PredicateOperator *pop = (PredicateOperator *) p;
	CompOp op = pop->getOp();

	if (pred->isFalse()) {
		op = negateOp(op);
	} else if (!pred->isTrue()) {
		ASSERT(0);
	}

	Set *s = var->getRange();
	if (s->isRange)
		return;

	uint size = s->getSize();
	uint64_t elemArray[size];
	uint count = 0;
	uint64_t cvalue = value->value;

	switch (op) {
	case SATC_LT: {
		for (uint i = 0; i < size; i++) {
			uint64_t val = s->getElement(i);
			if (val < cvalue)
				elemArray[count++] = val;
		}
		break;
	}
	case SATC_GT: {
		for (uint i = 0; i < size; i++) {
			uint64_t val = s->getElement(i);
			if (val > cvalue)
				elemArray[count++] = val;
		}
		break;
	}
	case SATC_LTE: {
		for (uint i = 0; i < size; i++) {
			uint64_t val = s->getElement(i);
			if (val <= cvalue)
				elemArray[count++] = val;
		}
		break;
	}
	case SATC_GTE: {
		for (uint i = 0; i < size; i++) {
			uint64_t val = s->getElement(i);
			if (val >= cvalue)
				elemArray[count++] = val;
		}
		break;
	}

	default:
		ASSERT(0);
	}
	if (size == count)
		return;

	Set *newset = solver->createSet(s->type, elemArray, count);
	solver->elemMap.remove(var);
	var->set = newset;
	solver->elemMap.put(var, var);

	if (count == 1) {
		ElementConst *elemconst = (ElementConst *) solver->getElementConst(s->type, elemArray[0]);
		replaceVarWithConst(pred, var, elemconst);
	}
}

void ElementOpt::constrainVarWithConst(BooleanPredicate *pred, ElementSet *var, ElementConst *value) {
	Set *s = var->getRange();
	if (s->isRange)
		return;
	uint size = s->getSize();
	uint64_t elemArray[size];
	uint count = 0;
	uint64_t cvalue = value->value;
	for (uint i = 0; i < size; i++) {
		uint64_t val = s->getElement(i);
		if (val != cvalue)
			elemArray[count++] = val;
	}
	if (size == count)
		return;

	Set *newset = solver->createSet(s->type, elemArray, count);
	solver->elemMap.remove(var);
	var->set = newset;
	solver->elemMap.put(var, var);

	if (count == 1) {
		ElementConst *elemconst = (ElementConst *) solver->getElementConst(s->type, elemArray[0]);
		replaceVarWithConst(pred, var, elemconst);
	}
}

void ElementOpt::replaceVarWithConst(BooleanPredicate *pred, ElementSet *var, ElementConst *value) {
	uint size = var->parents.getSize();
	for (uint i = 0; i < size; i++) {
		ASTNode *parent = var->parents.get(i);
		if (parent->type == PREDICATEOP && pred != parent) {
			BooleanPredicate *newpred = (BooleanPredicate *) parent;
			for (uint j = 0; j < newpred->inputs.getSize(); j++) {
				Element *e = newpred->inputs.get(j);
				if (e == var) {
					solver->boolMap.remove(newpred);
					newpred->inputs.set(j, value);
					solver->boolMap.put(newpred, newpred);
					if (newpred->isTrue() || newpred->isFalse())
						workList.push(newpred);
					break;
				}
			}
		}
	}
}
