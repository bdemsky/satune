#include "satencoder.h"
#include "structs.h"
#include "csolver.h"
#include "boolean.h"
#include "constraint.h"
#include "common.h"
#include "element.h"
#include "function.h"
#include "tableentry.h"
#include "table.h"
#include "order.h"
#include "predicate.h"


SATEncoder * allocSATEncoder() {
	SATEncoder *This=ourmalloc(sizeof (SATEncoder));
	This->varcount=1;
	return This;
}

void deleteSATEncoder(SATEncoder *This) {
	ourfree(This);
}

void initializeConstraintVars(CSolver* csolver, SATEncoder* This){
	/** We really should not walk the free list to generate constraint
			variables...walk the constraint tree instead.  Or even better
			yet, just do this as you need to during the encodeAllSATEncoder
			walk.  */

//	FIXME!!!!(); // Make sure Hamed sees comment above

	uint size = getSizeVectorElement(csolver->allElements);
	for(uint i=0; i<size; i++){
		Element* element = getVectorElement(csolver->allElements, i);
		generateElementEncodingVariables(This,getElementEncoding(element));
	}
}


Constraint * getElementValueConstraint(Element* This, uint64_t value) {
	switch(getElementEncoding(This)->type){
		case ONEHOT:
			ASSERT(0);
			break;
		case UNARY:
			ASSERT(0);
			break;
		case BINARYINDEX:
			ASSERT(0);
			break;
		case ONEHOTBINARY:
			return getElementValueBinaryIndexConstraint(This, value);
			break;
		case BINARYVAL:
			ASSERT(0);
			break;
		default:
			ASSERT(0);
			break;
	}
	return NULL;
}
Constraint * getElementValueBinaryIndexConstraint(Element* This, uint64_t value) {
	ASTNodeType type = GETELEMENTTYPE(This);
	ASSERT(type == ELEMSET || type == ELEMFUNCRETURN);
	ElementEncoding* elemEnc = getElementEncoding(This);
	for(uint i=0; i<elemEnc->encArraySize; i++){
		if( isinUseElement(elemEnc, i) && elemEnc->encodingArray[i]==value){
			return generateBinaryConstraint(elemEnc->numVars,
				elemEnc->variables, i);
		}
	}
	ASSERT(0);
	return NULL;
}

void encodeAllSATEncoder(CSolver *csolver, SATEncoder * This) {
	VectorBoolean *constraints=csolver->constraints;
	uint size=getSizeVectorBoolean(constraints);
	for(uint i=0;i<size;i++) {
		Boolean *constraint=getVectorBoolean(constraints, i);
		encodeConstraintSATEncoder(This, constraint);
	}
	
//	FIXME: Following line for element!
//	size = getSizeVectorElement(csolver->allElements);
//	for(uint i=0; i<size; i++){
//		Element* element = getVectorElement(csolver->allElements, i);
//		switch(GETELEMENTTYPE(element)){
//			case ELEMFUNCRETURN: 
//				encodeFunctionElementSATEncoder(This, (ElementFunction*) element);
//				break;
//			default:	
//				continue;
//				//ElementSets that aren't used in any constraints/functions
//				//will be eliminated.
//		}
//	}
}

Constraint * encodeConstraintSATEncoder(SATEncoder *This, Boolean *constraint) {
	switch(GETBOOLEANTYPE(constraint)) {
	case ORDERCONST:
		return encodeOrderSATEncoder(This, (BooleanOrder *) constraint);
	case BOOLEANVAR:
		return encodeVarSATEncoder(This, (BooleanVar *) constraint);
	case LOGICOP:
		return encodeLogicSATEncoder(This, (BooleanLogic *) constraint);
	case PREDICATEOP:
		return encodePredicateSATEncoder(This, (BooleanPredicate *) constraint);
	default:
		model_print("Unhandled case in encodeConstraintSATEncoder %u", GETBOOLEANTYPE(constraint));
		exit(-1);
	}
}

void getArrayNewVarsSATEncoder(SATEncoder* encoder, uint num, Constraint **carray) {
	for(uint i=0;i<num;i++)
		carray[i]=getNewVarSATEncoder(encoder);
}

Constraint * getNewVarSATEncoder(SATEncoder *This) {
	Constraint * var=allocVarConstraint(VAR, This->varcount);
	Constraint * varneg=allocVarConstraint(NOTVAR, This->varcount++);
	setNegConstraint(var, varneg);
	setNegConstraint(varneg, var);
	return var;
}

Constraint * encodeVarSATEncoder(SATEncoder *This, BooleanVar * constraint) {
	if (constraint->var == NULL) {
		constraint->var=getNewVarSATEncoder(This);
	}
	return constraint->var;
}

Constraint * encodeLogicSATEncoder(SATEncoder *This, BooleanLogic * constraint) {
	Constraint * array[getSizeArrayBoolean(&constraint->inputs)];
	for(uint i=0;i<getSizeArrayBoolean(&constraint->inputs);i++)
		array[i]=encodeConstraintSATEncoder(This, getArrayBoolean(&constraint->inputs, i));

	switch(constraint->op) {
	case L_AND:
		return allocArrayConstraint(AND, getSizeArrayBoolean(&constraint->inputs), array);
	case L_OR:
		return allocArrayConstraint(OR, getSizeArrayBoolean(&constraint->inputs), array);
	case L_NOT:
		ASSERT(constraint->numArray==1);
		return negateConstraint(array[0]);
	case L_XOR: {
		ASSERT(constraint->numArray==2);
		Constraint * nleft=negateConstraint(cloneConstraint(array[0]));
		Constraint * nright=negateConstraint(cloneConstraint(array[1]));
		return allocConstraint(OR,
													 allocConstraint(AND, array[0], nright),
													 allocConstraint(AND, nleft, array[1]));
	}
	case L_IMPLIES:
		ASSERT(constraint->numArray==2);
		return allocConstraint(IMPLIES, array[0], array[1]);
	default:
		model_print("Unhandled case in encodeLogicSATEncoder %u", constraint->op);
		exit(-1);
	}
}

Constraint * encodeOrderSATEncoder(SATEncoder *This, BooleanOrder * constraint) {
	switch( constraint->order->type){
		case PARTIAL:
			return encodePartialOrderSATEncoder(This, constraint);
		case TOTAL:
			return encodeTotalOrderSATEncoder(This, constraint);
		default:
			ASSERT(0);
	}
	return NULL;
}

Constraint * encodeTotalOrderSATEncoder(SATEncoder *This, BooleanOrder * boolOrder){
	ASSERT(boolOrder->order->type == TOTAL);
	HashTableBoolConst* boolToConsts = boolOrder->order->boolsToConstraints;
	if( containsBoolConst(boolToConsts, boolOrder) ){
		return getBoolConst(boolToConsts, boolOrder);
	} else {
		Constraint* constraint = getNewVarSATEncoder(This); 
		putBoolConst(boolToConsts,boolOrder, constraint);
		VectorBoolean* orderConstrs = &boolOrder->order->constraints;
		uint size= getSizeVectorBoolean(orderConstrs);
		for(uint i=0; i<size; i++){
			ASSERT(GETBOOLEANTYPE( getVectorBoolean(orderConstrs, i)) == ORDERCONST );
			BooleanOrder* tmp = (BooleanOrder*)getVectorBoolean(orderConstrs, i);
			BooleanOrder* newBool;
			Constraint* first, *second;
			if(tmp->second==boolOrder->first){
				newBool = (BooleanOrder*)allocBooleanOrder(tmp->order,tmp->first,boolOrder->second);
				first = encodeTotalOrderSATEncoder(This, tmp);
				second = constraint;
				
			}else if (boolOrder->second == tmp->first){
				newBool = (BooleanOrder*)allocBooleanOrder(tmp->order,boolOrder->first,tmp->second);
				first = constraint;
				second = encodeTotalOrderSATEncoder(This, tmp);
			}else
				continue;
			Constraint* transConstr= encodeTotalOrderSATEncoder(This, newBool);
			generateTransOrderConstraintSATEncoder(This, first, second, transConstr );
		}
		return constraint;
	}
	
	return NULL;
}

Constraint * generateTransOrderConstraintSATEncoder(SATEncoder *This, Constraint *first,Constraint *second,Constraint *third){
	//FIXME: first we should add the the constraint to the satsolver!
	return allocConstraint(IMPLIES, allocConstraint(AND, first, second), third);
}

Constraint * encodePartialOrderSATEncoder(SATEncoder *This, BooleanOrder * constraint){
	return NULL;
}

Constraint * encodePredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint) {
	switch(GETPREDICATETYPE(constraint) ){
		case TABLEPRED:
			return encodeTablePredicateSATEncoder(This, constraint);
		case OPERATORPRED:
			return encodeOperatorPredicateSATEncoder(This, constraint);
		default:
			ASSERT(0);
	}
	return NULL;
}

Constraint * encodeTablePredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint){
	switch(constraint->encoding.type){
		case ENUMERATEIMPLICATIONS:
			return encodeEnumTablePredicateSATEncoder(This, constraint);
		case CIRCUIT:
			ASSERT(0);
			break;
		default:
			ASSERT(0);
	}
	return NULL;
}

Constraint * encodeEnumTablePredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint){
	VectorTableEntry* entries = &(((PredicateTable*)constraint->predicate)->table->entries);
	uint size = getSizeVectorTableEntry(entries);
	for(uint i=0; i<size; i++){
		TableEntry* entry = getVectorTableEntry(entries, i);
		
	}
	return NULL;
}

Constraint * encodeOperatorPredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint){
	switch(constraint->encoding.type){
		case ENUMERATEIMPLICATIONS:
			break;
		case CIRCUIT:
			break;
		default:
			ASSERT(0);
	}
	return NULL;
}

Constraint* encodeFunctionElementSATEncoder(SATEncoder* encoder, ElementFunction *This){
	switch(GETFUNCTIONTYPE(This->function)){
		case TABLEFUNC:
			return encodeTableElementFunctionSATEncoder(encoder, This);
		case OPERATORFUNC:
			return encodeOperatorElementFunctionSATEncoder(encoder, This);
		default:
			ASSERT(0);
	}
	return NULL;
}

Constraint* encodeTableElementFunctionSATEncoder(SATEncoder* encoder, ElementFunction* This){
	switch(getElementFunctionEncoding(This)->type){
		case ENUMERATEIMPLICATIONS:
			return encodeEnumTableElemFunctionSATEncoder(encoder, This);
			break;
		case CIRCUIT:
			ASSERT(0);
			break;
		default:
			ASSERT(0);
	}
	return NULL;
}

Constraint* encodeOperatorElementFunctionSATEncoder(SATEncoder* encoder, ElementFunction* This){
	//FIXME: for now it just adds/substracts inputs exhustively
	return NULL;
}

Constraint* encodeEnumTableElemFunctionSATEncoder(SATEncoder* encoder, ElementFunction* This){
	ASSERT(GETFUNCTIONTYPE(This->function)==TABLEFUNC);
	ArrayElement* elements= &This->inputs;
	Table* table = ((FunctionTable*) (This->function))->table;
	uint size = getSizeVectorTableEntry(&table->entries);
	Constraint* constraints[size]; //FIXME: should add a space for the case that didn't match any entries
	for(uint i=0; i<size; i++){
		TableEntry* entry = getVectorTableEntry(&table->entries, i);
		uint inputNum =getSizeArrayElement(elements);
		Element* el= getArrayElement(elements, i);
		Constraint* carray[inputNum];
		for(uint j=0; j<inputNum; j++){
			carray[inputNum] = getElementValueConstraint(el, entry->inputs[j]);
		}
		Constraint* row= allocConstraint(IMPLIES, allocArrayConstraint(AND, inputNum, carray),
			getElementValueBinaryIndexConstraint((Element*)This, entry->output));
		constraints[i]=row;
	}
	Constraint* result = allocArrayConstraint(OR, size, constraints);
	return result;
}
