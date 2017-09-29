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
	BooleanVar* This = this;
	serializer->mywrite(&This, sizeof(BooleanVar*));
	serializer->mywrite(&vtype, sizeof(VarType));
}

void BooleanVar::print(){
        model_println("BooleanVar");
}

void BooleanOrder::serialize(Serializer* serializer){
	if(serializer->isSerialized(this))
		return;
	serializer->addObject(this);
	order->serialize(serializer);
	
	serializer->mywrite(&type, sizeof(ASTNodeType));
	BooleanOrder* This = this;
	serializer->mywrite(&This, sizeof(BooleanOrder*));
	serializer->mywrite(&order, sizeof(Order*));
	serializer->mywrite(&first, sizeof(uint64_t));
	serializer->mywrite(&second, sizeof(uint64_t));
}

void BooleanOrder::print(){
	model_println("{BooleanOrder: First= %lu, Second = %lu on Order:", first, second);
	order->print();
        model_println("}\n");
}

void BooleanPredicate::serialize(Serializer* serializer){
	if(serializer->isSerialized(this))
		return;
	serializer->addObject(this);
	
	predicate->serialize(serializer);
	uint size = inputs.getSize();
	for(uint i=0; i<size; i++){
		Element* input = inputs.get(i);
		input->serialize(serializer);
	}
	serializeBooleanEdge(serializer, undefStatus);
	
	serializer->mywrite(&type, sizeof(ASTNodeType));
	BooleanPredicate* This = this;
	serializer->mywrite(&This, sizeof(BooleanPredicate*));
	serializer->mywrite(&predicate, sizeof(Predicate *));
	serializer->mywrite(&size, sizeof(uint));
	for(uint i=0; i<size; i++){
		Element *input = inputs.get(i);
		serializer->mywrite(&input, sizeof(Element *));
	}
	Boolean* undefStat = undefStatus!= BooleanEdge(NULL)?undefStatus.getRaw() : NULL;
	serializer->mywrite(&undefStat, sizeof(Boolean*));
}

void BooleanPredicate::print(){
	model_println("{BooleanPredicate:");
        predicate->print();
	model_println("elements:");
        uint size = inputs.getSize();
	for(uint i=0; i<size; i++){
		Element *input = inputs.get(i);
		input->print();
	}
        model_println("}\n");
}

void BooleanLogic::serialize(Serializer* serializer){
	if(serializer->isSerialized(this))
		return;
	serializer->addObject(this);
	uint size = inputs.getSize();
	for(uint i=0; i<size; i++){
		BooleanEdge input = inputs.get(i);
		serializeBooleanEdge(serializer, input);
	}
	serializer->mywrite(&type, sizeof(ASTNodeType));
	BooleanLogic* This = this;
	serializer->mywrite(&This, sizeof(BooleanLogic*));
	serializer->mywrite(&op, sizeof(LogicOp));
	serializer->mywrite(&size, sizeof(uint));
	for(uint i=0; i<size; i++){
		Boolean* input = inputs.get(i).getRaw();
		serializer->mywrite(&input, sizeof(Boolean*));
	}
}

void BooleanLogic::print(){
	model_println("{BooleanLogic: %s", 
                op ==SATC_AND? "AND": op == SATC_OR? "OR": op==SATC_NOT? "NOT":
                op == SATC_XOR? "XOR" : op==SATC_IFF? "IFF" : "IMPLIES");
        uint size = inputs.getSize();
	for(uint i=0; i<size; i++){
		BooleanEdge input = inputs.get(i);
                input.getBoolean()->print();
	}
        model_println("}\n");
}

