#include "boolean.h"
#include "structs.h"
#include "csolver.h"
#include "element.h"
#include "order.h"

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

BooleanPredicate::~BooleanPredicate() {
}
