#include "boolean.h"
#include "structs.h"
#include "csolver.h"
#include "element.h"
#include "order.h"
#include "predicate.h"

Boolean::Boolean(ASTNodeType _type) :
	ASTNode(_type),
	polarity(P_UNDEFINED),
	boolVal(BV_UNDEFINED),
	parents() {
}

BooleanVar::BooleanVar(VarType t) :
	Boolean(BOOLEANVAR),
	vtype(t),
	var(E_NULL) {
}

BooleanOrder::BooleanOrder(Order *_order, uint64_t _first, uint64_t _second) :
	Boolean(ORDERCONST),
	order(_order),
	first(_first),
	second(_second) {
	order->constraints.push(this);
}

BooleanPredicate::BooleanPredicate(Predicate *_predicate, Element **_inputs, uint _numInputs, Boolean *_undefinedStatus) :
	Boolean(PREDICATEOP),
	predicate(_predicate),
	encoding(this),
	inputs(_inputs, _numInputs),
	undefStatus(_undefinedStatus) {
	for (uint i = 0; i < _numInputs; i++) {
		GETELEMENTPARENTS(_inputs[i])->push(this);
	}
}

BooleanLogic::BooleanLogic(CSolver *solver, LogicOp _op, Boolean **array, uint asize) :
	Boolean(LOGICOP),
	op(_op),
	inputs(array, asize) {
}

Boolean *BooleanVar::clone(CSolver *solver, CloneMap *map) {
	Boolean *b = (Boolean *) map->get(this);
	if (b != NULL)
		return b;
	Boolean *bvar = solver->getBooleanVar(type);
	map->put(this, bvar);
	return bvar;

}

Boolean *BooleanOrder::clone(CSolver *solver, CloneMap *map) {
	Order *ordercopy = order->clone(solver, map);
	return solver->orderConstraint(ordercopy, first, second);
}

Boolean *BooleanLogic::clone(CSolver *solver, CloneMap *map) {
	Boolean *array[inputs.getSize()];
	for (uint i = 0; i < inputs.getSize(); i++) {
		array[i] = inputs.get(i)->clone(solver, map);
	}
	return solver->applyLogicalOperation(op, array, inputs.getSize());
}

Boolean *BooleanPredicate::clone(CSolver *solver, CloneMap *map) {
	Element *array[inputs.getSize()];
	for (uint i = 0; i < inputs.getSize(); i++) {
		array[i] = inputs.get(i)->clone(solver, map);
	}
	Predicate *pred = predicate->clone(solver, map);
	Boolean *defstatus = (undefStatus != NULL) ? undefStatus->clone(solver, map) : NULL;

	return solver->applyPredicateTable(pred, array, inputs.getSize(), defstatus);
}
