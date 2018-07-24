#include "elementopt.h"
#include "csolver.h"
#include "tunable.h"
#include "iterator.h"
#include "boolean.h"
#include "element.h"
#include "predicate.h"

ElementOpt::ElementOpt(CSolver *_solver)
	: Transform(_solver)
{
}

ElementOpt::~ElementOpt() {
}

void ElementOpt::doTransform() {
	if (solver->getTuner()->getTunable(ELEMENTOPT, &onoff) == 0)
		return;
	
	SetIteratorBooleanEdge *iterator = solver->getConstraints();
	while (iterator->hasNext()) {
		BooleanEdge constraint = iterator->next();
		if (constraint->type == PREDICATEOP)
			workList.push((BooleanPredicate *)constraint.getBoolean());
	}
	while (workList.getSize() != 0) {
		BooleanPredicate * pred = workList.last(); workList.pop();
		processPredicate(pred);
	}
	delete iterator;
}

void ElementOpt::processPredicate(BooleanPredicate * pred) {
	uint numInputs = pred->inputs.getSize();
	if (numInputs != 2)
		return;

	Predicate * p = pred->getPredicate();
	if (p->type == TABLEPRED)
			return;

	PredicateOperator * pop = (PredicateOperator *) p;
	CompOp op = pop->getOp();

	Element * left = pred->inputs.get(0);
	Element * right = pred->inputs.get(1);

	if (left->type == ELEMCONST) {
		Element * tmp = left;
		left = right;
		right = tmp;
		op = flipOp(op);
	} else if (right->type != ELEMCONST)
		return;

	if (left->type !=ELEMSET)
		return;
	
	if (op == SATC_EQUALS) {
		handlePredicateEquals(pred, (ElementSet *) left, (ElementConst *) right);
	} else {
		handlePredicateInequality(pred, (ElementSet *) left, (ElementConst *) right);
	}
}

void ElementOpt::handlePredicateEquals(BooleanPredicate *pred, ElementSet *left, ElementConst *right) {
	if (pred->isTrue()) {
		replaceVarWithConst(pred, left, right);
	} else if (pred->isFalse()) {
		
	} else ASSERT(0);
}

void ElementOpt::handlePredicateInequality(BooleanPredicate *pred, ElementSet *left, ElementConst *right) {

}

void ElementOpt::replaceVarWithConst(BooleanPredicate * pred, ElementSet *var, ElementConst * value) {
	uint size = var->parents.getSize();
	for(uint i=0; i < size; i++) {
		ASTNode * parent = var->parents.get(i);
		if (parent->type == PREDICATEOP && pred != parent) {
			BooleanPredicate * newpred = (BooleanPredicate *) parent;
			for(uint j=0; j < newpred->inputs.getSize(); j++) {
				Element * e = newpred->inputs.get(j);
				if (e == var) {
					solver->boolMap.remove(newpred);
					newpred->inputs.set(j, value);
					solver->boolMap.put(newpred, newpred);
					workList.push(newpred);
					break;
				}
			}
		}
	}
}
