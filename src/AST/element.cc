#include "element.h"
#include "structs.h"
#include "set.h"
#include "constraint.h"
#include "function.h"
#include "table.h"

Element::Element(ASTNodeType _type) : ASTNode(_type) {
	initDefVectorASTNode(GETELEMENTPARENTS(this));
	initElementEncoding(&encoding, (Element *) this);
}

ElementSet::ElementSet(Set *s) : Element(ELEMSET), set(s) {
}

ElementFunction::ElementFunction(Function *_function, Element **array, uint numArrays, Boolean *_overflowstatus) : Element(ELEMFUNCRETURN), function(_function), overflowstatus(_overflowstatus) {
	initArrayInitElement(&inputs, array, numArrays);
	for (uint i = 0; i < numArrays; i++)
		pushVectorASTNode(GETELEMENTPARENTS(array[i]), this);
	initFunctionEncoding(&functionencoding, this);
}

ElementConst::ElementConst(uint64_t _value, VarType _type) : Element(ELEMCONST), value(_value) {
	uint64_t array[]={value};
	set = allocSet(_type, array, 1);
}

Set *getElementSet(Element *This) {
	switch (GETELEMENTTYPE(This)) {
	case ELEMSET:
		return ((ElementSet *)This)->set;
	case ELEMCONST:
		return ((ElementConst *)This)->set;
	case ELEMFUNCRETURN: {
		Function *func = ((ElementFunction *)This)->function;
		switch (GETFUNCTIONTYPE(func)) {
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

ElementFunction::~ElementFunction() {
	deleteInlineArrayElement(&inputs);
	deleteFunctionEncoding(&functionencoding);
}

ElementConst::~ElementConst() {
	deleteSet(set);
}

Element::~Element() {
	deleteElementEncoding(&encoding);
	deleteVectorArrayASTNode(GETELEMENTPARENTS(this));
}
