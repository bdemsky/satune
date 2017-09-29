#include "function.h"
#include "table.h"
#include "set.h"
#include "csolver.h"
#include "serializer.h"

FunctionOperator::FunctionOperator(ArithOp _op, Set **domain, uint numDomain, Set *_range, OverFlowBehavior _overflowbehavior) :
	Function(OPERATORFUNC),
	op(_op),
	domains(domain, numDomain),
	range(_range),
	overflowbehavior(_overflowbehavior) {
}

FunctionTable::FunctionTable (Table *_table, UndefinedBehavior _undefBehavior) :
	Function(TABLEFUNC),
	table(_table),
	undefBehavior(_undefBehavior) {
}

uint64_t FunctionOperator::applyFunctionOperator(uint numVals, uint64_t *values) {
	ASSERT(numVals == 2);
	switch (op) {
	case SATC_ADD:
		return values[0] + values[1];
		break;
	case SATC_SUB:
		return values[0] - values[1];
		break;
	default:
		ASSERT(0);
	}
}

bool FunctionOperator::isInRangeFunction(uint64_t val) {
	return range->exists(val);
}

Function *FunctionOperator::clone(CSolver *solver, CloneMap *map) {
	Function *f = (Function *) map->get(this);
	if (f != NULL)
		return f;

	Set *array[domains.getSize()];
	for (uint i = 0; i < domains.getSize(); i++) {
		array[i] = domains.get(i)->clone(solver, map);
	}
	Set *rcopy = range->clone(solver, map);
	f = solver->createFunctionOperator(op, array, domains.getSize(), rcopy, overflowbehavior);
	map->put(this, f);
	return f;
}

Function *FunctionTable::clone(CSolver *solver, CloneMap *map) {
	Function *f = (Function *) map->get(this);
	if (f != NULL)
		return f;

	Table *tcopy = table->clone(solver, map);
	f = solver->completeTable(tcopy, undefBehavior);
	map->put(this, f);
	return f;
}

Set * FunctionTable::getRange() {
	return table->getRange();
}

void FunctionTable::serialize(Serializer* serializer){
	if(serializer->isSerialized(this))
		return;
	serializer->addObject(this);
	
	table->serialize(serializer);
	
	ASTNodeType type = FUNCTABLETYPE;	
	serializer->mywrite(&type, sizeof(ASTNodeType));
	FunctionTable* This = this;
	serializer->mywrite(&This, sizeof(FunctionTable*));
	serializer->mywrite(&table, sizeof(Table *));
	serializer->mywrite(&undefBehavior, sizeof(UndefinedBehavior));
	
}

void FunctionTable::print(){
	model_println("{FunctionTable:");
        table->print();
        model_println("}\n");
	
}

void FunctionOperator::serialize(Serializer* serializer){
	if(serializer->isSerialized(this))
		return;
	serializer->addObject(this);
	
	uint size = domains.getSize();
	for(uint i=0; i<size; i++){
		Set* domain = domains.get(i);
		domain->serialize(serializer);
	}
	range->serialize(serializer);
	
	ASTNodeType nodeType = FUNCOPTYPE; 
	serializer->mywrite(&nodeType, sizeof(ASTNodeType));
	FunctionOperator* This = this;
	serializer->mywrite(&This, sizeof(FunctionOperator*));
	serializer->mywrite(&op, sizeof(ArithOp));
	serializer->mywrite(&size, sizeof(uint));
	for(uint i=0; i<size; i++){
		Set *domain = domains.get(i);
		serializer->mywrite(&domain, sizeof(Set *));
	}
	serializer->mywrite(&range, sizeof(Set *));
	serializer->mywrite(&overflowbehavior, sizeof(OverFlowBehavior));
}

void FunctionOperator::print(){
	model_println("{FunctionOperator: %s}", op == SATC_ADD? "ADD": "SUB" );
}