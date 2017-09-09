#include "element.h"
#include "structs.h"
#include "set.h"
#include "constraint.h"
#include "function.h"
#include "table.h"
#include "csolver.h"

Element::Element(ASTNodeType _type) :
	ASTNode(_type),
	encoding(this) {
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
	return solver->getElementConst(type, value);
}

Element *ElementSet::clone(CSolver *solver, CloneMap *map) {
	Element *e = (Element *) map->get(this);
	if (e != NULL)
		return e;
	e = solver->getElementVar(set->clone(solver, map));
	map->put(e, e);
	return e;
}

Element *ElementFunction::clone(CSolver *solver, CloneMap *map) {
	Element *array[inputs.getSize()];
	for (uint i = 0; i < inputs.getSize(); i++) {
		array[i] = inputs.get(i)->clone(solver, map);
	}
	Element *e = solver->applyFunction(function->clone(solver, map), array, inputs.getSize(), overflowstatus->clone(solver, map));
	return e;
}

void ElementFunction::updateParents() {
	for(uint i=0;i < inputs.getSize(); i++) inputs.get(i)->parents.push(this);
}

Set * ElementFunction::getRange() {
	return function->getRange();
}
