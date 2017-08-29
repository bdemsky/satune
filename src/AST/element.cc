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
	idNumber(0) {
}

ElementSet::ElementSet(Set *s) :
	Element(ELEMSET),
	set(s) {
}

ElementFunction::ElementFunction(Function *_function, Element **array, uint numArrays, Boolean *_overflowstatus) :
	Element(ELEMFUNCRETURN),
	function(_function),
	inputs(array, numArrays),
	overflowstatus(_overflowstatus),
	functionencoding(this) {
	for (uint i = 0; i < numArrays; i++)
		array[i]->parents.push(this);
}

ElementConst::ElementConst(uint64_t _value, VarType _type, Set *_set) :
	Element(ELEMCONST),
	set(_set),
	value(_value) {
}

Set *getElementSet(Element *This) {
	switch (This->type) {
	case ELEMSET:
		return ((ElementSet *)This)->set;
	case ELEMCONST:
		return ((ElementConst *)This)->set;
	case ELEMFUNCRETURN: {
		Function *func = ((ElementFunction *)This)->function;
		switch (func->type) {
		case TABLEFUNC:
			return ((FunctionTable *)func)->table->range;
		case OPERATORFUNC:
			return ((FunctionOperator *)func)->range;
		default:
			ASSERT(0);
		}
	}
	default:
		ASSERT(0);
	}
	ASSERT(0);
	return NULL;
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
