#include "element.h"
#include "structs.h"
#include "set.h"
#include "constraint.h"
#include "function.h"
#include "table.h"
#include "csolver.h"

Element::Element(ASTNodeType _type) :
	ASTNode(_type),
	encoding(this),
	anyValue(false) {
}

ElementSet::ElementSet(Set *s) :
	Element(ELEMSET),
	set(s) {
}

ElementSet::ElementSet(ASTNodeType _type, Set *s) :
	Element(_type),
	set(s) {
}

ElementFunction::ElementFunction(Function *_function, Element **array, uint numArrays, BooleanEdge _overflowstatus) :
	Element(ELEMFUNCRETURN),
	inputs(array, numArrays),
	overflowstatus(_overflowstatus),
	functionencoding(this),
	function(_function) {
}

ElementConst::ElementConst(uint64_t _value, Set *_set) :
	ElementSet(ELEMCONST, _set),
	value(_value) {
}

Element *ElementConst::clone(CSolver *solver, CloneMap *map) {
	Element *e = solver->getElementConst(type, value);
	e->anyValue = anyValue;
	return e;
}

Element *ElementSet::clone(CSolver *solver, CloneMap *map) {
	Element *e = (Element *) map->get(this);
	if (e != NULL)
		return e;
	e = solver->getElementVar(set->clone(solver, map));
	map->put(this, e);
	e->anyValue = anyValue;
	return e;
}

Element *ElementFunction::clone(CSolver *solver, CloneMap *map) {
	Element *array[inputs.getSize()];
	for (uint i = 0; i < inputs.getSize(); i++) {
		array[i] = inputs.get(i)->clone(solver, map);
	}
	BooleanEdge ofstatus = overflowstatus ? cloneEdge(solver, map, overflowstatus) : BooleanEdge();
	Element *e = solver->applyFunction(function->clone(solver, map), array, inputs.getSize(), ofstatus);
	e->anyValue = anyValue;
	return e;
}

void ElementFunction::updateParents() {
	for (uint i = 0; i < inputs.getSize(); i++) inputs.get(i)->parents.push(this);
	if (overflowstatus)
		overflowstatus->parents.push(this);
}

Set *ElementFunction::getRange() {
	return function->getRange();
}

void ElementSet::serialize(Serializer *serializer) {
	if (serializer->isSerialized(this))
		return;
	serializer->addObject(this);

	set->serialize(serializer);

	serializer->mywrite(&type, sizeof(ASTNodeType));
	serializer->mywrite(&anyValue, sizeof(bool));
	ElementSet *This = this;
	serializer->mywrite(&This, sizeof(ElementSet *));
	serializer->mywrite(&set, sizeof(Set *));
}

void ElementSet::print() {
	model_print("{ElementSet<%p>:", this);
	set->print();
	model_print(" %p ", this);
	getElementEncoding()->print();
	model_print("}");
}

void ElementConst::serialize(Serializer *serializer) {
	if (serializer->isSerialized(this))
		return;
	serializer->addObject(this);

	set->serialize(serializer);

	serializer->mywrite(&type, sizeof(ASTNodeType));
	serializer->mywrite(&anyValue, sizeof(bool));
	ElementSet *This = this;
	serializer->mywrite(&This, sizeof(ElementSet *));
	VarType type = set->getType();
	serializer->mywrite(&type, sizeof(VarType));
	serializer->mywrite(&value, sizeof(uint64_t));
}

void ElementConst::print() {
	model_print("{ElementConst<%p>: %" PRIu64 "}\n", this, value);
}

void ElementFunction::serialize(Serializer *serializer) {
	if (serializer->isSerialized(this))
		return;
	serializer->addObject(this);

	function->serialize(serializer);
	uint size = inputs.getSize();
	for (uint i = 0; i < size; i++) {
		Element *input = inputs.get(i);
		input->serialize(serializer);
	}
	serializeBooleanEdge(serializer, overflowstatus);

	serializer->mywrite(&type, sizeof(ASTNodeType));
	serializer->mywrite(&anyValue, sizeof(bool));
	ElementFunction *This = this;
	serializer->mywrite(&This, sizeof(ElementFunction *));
	serializer->mywrite(&function, sizeof(Function *));
	serializer->mywrite(&size, sizeof(uint));
	for (uint i = 0; i < size; i++) {
		Element *input = inputs.get(i);
		serializer->mywrite(&input, sizeof(Element *));
	}
	Boolean *overflowstat = overflowstatus.getRaw();
	serializer->mywrite(&overflowstat, sizeof(Boolean *));
}

void ElementFunction::print() {
	model_print("{ElementFunction<%p>:\n", this);
	function->print();
	model_print("OverFlow Boolean Flag:\n");
	overflowstatus.getBoolean()->print();
	model_print("Range:\n");
	getRange()->print();
	model_print("Elements:\n");
	uint size = inputs.getSize();
	for (uint i = 0; i < size; i++) {
		Element *input = inputs.get(i);
		input->print();
	}
	model_print("}\n");
}
