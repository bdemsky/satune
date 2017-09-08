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

BooleanConst::BooleanConst(bool _isTrue) :
	Boolean(BOOLCONST),
	istrue(_isTrue) {
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

BooleanPredicate::BooleanPredicate(Predicate *_predicate, Element **_inputs, uint _numInputs, BooleanEdge _undefinedStatus) :
	Boolean(PREDICATEOP),
	predicate(_predicate),
	encoding(this),
	inputs(_inputs, _numInputs),
	undefStatus(_undefinedStatus) {
}

BooleanLogic::BooleanLogic(CSolver *solver, LogicOp _op, BooleanEdge *array, uint asize) :
	Boolean(LOGICOP),
	op(_op),
	replaced(false),
	inputs(array, asize) {
}

BooleanEdge cloneEdge(CSolver *solver, CloneMap *map, BooleanEdge e) {
	bool isnegated=e.isNegated();
	Boolean *b=e->clone(solver, map);
	BooleanEdge be=BooleanEdge(b);
	return isnegated ? be.negate() : be;
}

Boolean *BooleanConst::clone(CSolver *solver, CloneMap *map) {
	return solver->getBooleanTrue().getRaw();
}

Boolean *BooleanVar::clone(CSolver *solver, CloneMap *map) {
	Boolean *b = (Boolean *) map->get(this);
	if (b != NULL)
		return b;
	BooleanEdge bvar = solver->getBooleanVar(type);
	Boolean * base=bvar.getRaw();
	map->put(this, base);
	return base;
}

Boolean *BooleanOrder::clone(CSolver *solver, CloneMap *map) {
	Order *ordercopy = order->clone(solver, map);
	return solver->orderConstraint(ordercopy, first, second).getRaw();
}

Boolean *BooleanLogic::clone(CSolver *solver, CloneMap *map) {
	BooleanEdge array[inputs.getSize()];
	for (uint i = 0; i < inputs.getSize(); i++) {
		array[i] = cloneEdge(solver, map, inputs.get(i));
	}
	return solver->applyLogicalOperation(op, array, inputs.getSize()).getRaw();
}

Boolean *BooleanPredicate::clone(CSolver *solver, CloneMap *map) {
	Element *array[inputs.getSize()];
	for (uint i = 0; i < inputs.getSize(); i++) {
		array[i] = inputs.get(i)->clone(solver, map);
	}
	Predicate *pred = predicate->clone(solver, map);
	BooleanEdge defstatus = undefStatus ? cloneEdge(solver, map, undefStatus) : BooleanEdge();

	return solver->applyPredicateTable(pred, array, inputs.getSize(), defstatus).getRaw();
}

void BooleanPredicate::updateParents() {
	for(uint i=0;i < inputs.getSize(); i++) inputs.get(i)->parents.push(this);
}

void BooleanLogic::updateParents() {
	for(uint i=0;i < inputs.getSize(); i++) inputs.get(i)->parents.push(this);
}

void BooleanVar::serialize(Serializer* serializer){
	if(serializer->isSerialized(this))
		return;
	serializer->addObject(this);
	serializer->mywrite(&type, sizeof(ASTNodeType));
	serializer->mywrite(this, sizeof(BooleanVar*));
	serializer->mywrite(&vtype, sizeof(VarType));
}

void BooleanOrder::serialize(Serializer* serializer){
	if(serializer->isSerialized(this))
		return;
	serializer->addObject(this);
//	order->serialize(serializer);
	serializer->mywrite(&type, sizeof(ASTNodeType));
	serializer->mywrite(this, sizeof(BooleanOrder*));
	serializer->mywrite(&order, sizeof(Order*));
	serializer->mywrite(&first, sizeof(uint64_t));
	serializer->mywrite(&second, sizeof(uint64_t));
}
