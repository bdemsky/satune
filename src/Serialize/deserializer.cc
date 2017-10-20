
/* 
 * File:   deserializer.cc
 * Author: hamed
 * 
 * Created on September 7, 2017, 6:08 PM
 */

#include "deserializer.h"
#include "csolver.h"
#include "unistd.h"
#include "fcntl.h"
#include "predicate.h"
#include "table.h"
#include "element.h"
#include "mutableset.h"

Deserializer::Deserializer(const char* file):
	solver(new CSolver())
{
	filedesc = open(file, O_RDONLY);
 
	if (filedesc < 0) {
		exit(-1);
	}
}

Deserializer::~Deserializer() {
	delete solver;
	
	if (-1 == close(filedesc)){
		exit(-1);
	}
}

ssize_t Deserializer::myread(void* __buf, size_t __nbytes){
	return read (filedesc, __buf, __nbytes);
}

CSolver * Deserializer::deserialize(){
	ASTNodeType nodeType;
	while(myread(&nodeType, sizeof(ASTNodeType) ) >0){
		switch(nodeType){
			case BOOLEANEDGE:
				deserializeBooleanEdge();
				break;
			case BOOLEANVAR:
				deserializeBooleanVar();
				break;
			case ORDERCONST:
				deserializeBooleanOrder();
				break;
			case ORDERTYPE:
				deserializeOrder();
				break;
			case SETTYPE:
				deserializeSet();
				break;
			case LOGICOP:
				deserializeBooleanLogic();
				break;
			case PREDICATEOP:
				deserializeBooleanPredicate();
				break;
			case PREDTABLETYPE:
				deserializePredicateTable();
				break;
			case PREDOPERTYPE:
				deserializePredicateOperator();
				break;
			case TABLETYPE:
				deserializeTable();
				break;
			case ELEMSET:
				deserializeElementSet();
				break;
			case ELEMCONST:
				deserializeElementConst();
				break;
			case ELEMFUNCRETURN:
				deserializeElementFunction();
				break;
			case FUNCOPTYPE:
				deserializeFunctionOperator();
				break;
			case FUNCTABLETYPE:
				deserializeFunctionTable();
				break;
			default:
				ASSERT(0);
		}
	}
	return solver;
}

void Deserializer::deserializeBooleanEdge(){
	Boolean *b;
	myread(&b, sizeof(Boolean*));
	BooleanEdge tmp(b);
	bool isNegated = tmp.isNegated();
	ASSERT(map.contains(tmp.getBoolean()));
	b = (Boolean*) map.get(tmp.getBoolean());
	BooleanEdge res(b);
	solver->addConstraint(isNegated?res.negate():res);
}

void Deserializer::deserializeBooleanVar(){
	BooleanVar *b;
	myread(&b, sizeof(BooleanVar*));
	VarType vtype;
	myread(&vtype, sizeof(VarType));
	map.put(b, solver->getBooleanVar(vtype).getBoolean());
}

void Deserializer::deserializeBooleanOrder(){
	BooleanOrder* bo_ptr;
	myread(&bo_ptr, sizeof(BooleanOrder*));
	Order* order;
	myread(&order, sizeof(Order*));
	ASSERT(map.contains(order));
	order  = (Order*) map.get(order);
	uint64_t first;
	myread(&first, sizeof(uint64_t));
	uint64_t second;
	myread(&second, sizeof(uint64_t));
	map.put(bo_ptr, solver->orderConstraint(order, first, second).getBoolean());
}

void Deserializer::deserializeOrder(){
	Order* o_ptr;
	myread(&o_ptr, sizeof(Order*));
	OrderType type;
	myread(&type, sizeof(OrderType));
	Set * set_ptr;
	myread(&set_ptr, sizeof(Set *));
	ASSERT(map.contains(set_ptr));
	Set* set  = (Set*) map.get(set_ptr);
	map.put(o_ptr, solver->createOrder(type, set));
}

void Deserializer::deserializeSet(){
	Set *s_ptr;
	myread(&s_ptr, sizeof(Set*));
	VarType type;
	myread(&type, sizeof(VarType));
	bool isRange;
	myread(&isRange, sizeof(bool));
        bool isMutable;
	myread(&isMutable, sizeof(bool));
        if(isRange){
                uint64_t low;
                myread(&low, sizeof(uint64_t));
                uint64_t high;
                myread(&high, sizeof(uint64_t));
                map.put(s_ptr, new Set(type, low, high));
        } else{
                Set *set;
                if(isMutable){
                        set = new MutableSet(type);
                }
                uint size;
                myread(&size, sizeof(uint));
                Vector<uint64_t> members;
                for(uint i=0; i<size; i++){
                        uint64_t mem;
                        myread(&mem, sizeof(uint64_t));
                        if(isMutable) {
                                ((MutableSet*) set)->addElementMSet(mem);
                        }else {
                                members.push(mem);
                        }
                }
                if(!isMutable){
                        set = solver->createSet(type, members.expose(), size);
                }
                map.put(s_ptr, set);
        }
	
}

void Deserializer::deserializeBooleanLogic(){
	BooleanLogic *bl_ptr;
	myread(&bl_ptr, sizeof(BooleanLogic *));
	LogicOp op;
	myread(&op, sizeof(LogicOp));
	uint size;
	myread(&size, sizeof(uint));
	Vector<BooleanEdge> members;
	for(uint i=0; i<size; i++){
		Boolean* member;
		myread(&member, sizeof(Boolean *));
		BooleanEdge tmp(member);
		bool isNegated = tmp.isNegated();
		ASSERT(map.contains(tmp.getBoolean()));
		member = (Boolean*) map.get(tmp.getBoolean());
		BooleanEdge res(member);
		members.push( isNegated?res.negate():res );
	}
	map.put(bl_ptr, solver->applyLogicalOperation(op, members.expose(), size).getBoolean());
}

void Deserializer::deserializeBooleanPredicate(){
	BooleanPredicate *bp_ptr;
	myread(&bp_ptr, sizeof(BooleanPredicate *));
	Predicate* predicate;
	myread(&predicate, sizeof(Predicate*));
	ASSERT(map.contains(predicate));
	predicate = (Predicate*) map.get(predicate);
	uint size;
	myread(&size, sizeof(uint));
	Vector<Element*> members;
	for(uint i=0; i<size; i++){
		Element* input;
		myread(&input, sizeof(Element *));
		ASSERT(map.contains(input));
		input = (Element*) map.get(input);
		members.push(input);
	}
	
	Boolean* stat_ptr;
	myread(&stat_ptr, sizeof(Boolean *));
	BooleanEdge undefStatus;
	if(stat_ptr != NULL){
		BooleanEdge tmp(stat_ptr);
		bool isNegated = tmp.isNegated();
		ASSERT(map.contains(tmp.getBoolean()));
		stat_ptr = (Boolean*) map.get(tmp.getBoolean());
		BooleanEdge res(stat_ptr);
		undefStatus = isNegated?res.negate():res;
	} else {
		undefStatus = NULL;
	}
	map.put(bp_ptr, solver->applyPredicateTable(predicate, members.expose(), size, undefStatus).getBoolean());
}

void Deserializer::deserializePredicateTable(){
	PredicateTable *pt_ptr;
	myread(&pt_ptr, sizeof(PredicateTable *));
	Table* table;
	myread(&table, sizeof(Table*));
	ASSERT(map.contains(table));
	table = (Table*) map.get(table);
	UndefinedBehavior undefinedbehavior;
	myread(&undefinedbehavior, sizeof(UndefinedBehavior));
	
	map.put(pt_ptr, solver->createPredicateTable(table, undefinedbehavior));
}

void Deserializer::deserializePredicateOperator(){
	PredicateOperator *po_ptr;
	myread(&po_ptr, sizeof(PredicateOperator *));
	CompOp op;
	myread(&op, sizeof(CompOp));
	uint size;
	myread(&size, sizeof(uint));
	Vector<Set*> domains;
	for(uint i=0; i<size; i++){
		Set* domain;
		myread(&domain, sizeof(Set*));
		ASSERT(map.contains(domain));
		domain = (Set*) map.get(domain);
		domains.push(domain);
	}

	map.put(po_ptr, solver->createPredicateOperator(op, domains.expose(), size));
}

void Deserializer::deserializeTable(){
	Table *t_ptr;
	myread(&t_ptr, sizeof(Table *));
	uint size;
	myread(&size, sizeof(uint));
	Vector<Set*> domains;
	for(uint i=0; i<size; i++){
		Set* domain;
		myread(&domain, sizeof(Set*));
		ASSERT(map.contains(domain));
		domain = (Set*) map.get(domain);
		domains.push(domain);
	}
	Set* range;
	myread(&range, sizeof(Set*));
	if(range != NULL){
		ASSERT(map.contains(range));
		range = (Set*) map.get(range);
	}
	Table* table = solver->createTable(domains.expose(), size, range);
	myread(&size, sizeof(uint));
	for(uint i=0; i<size; i++){
		uint64_t output;
		myread(&output, sizeof(uint64_t));
		uint inputSize;
		myread(&inputSize, sizeof(uint));
		Vector<uint64_t> inputs;
		inputs.setSize(inputSize);
		myread(inputs.expose(), sizeof(uint64_t)*inputSize);
		table->addNewTableEntry(inputs.expose(), inputSize, output);
	}
	
	map.put(t_ptr, table);
}


void Deserializer::deserializeElementSet(){
	ElementSet* es_ptr;
	myread(&es_ptr, sizeof(ElementSet*));
	Set * set;
	myread(&set, sizeof(Set *));
	ASSERT(map.contains(set));
	set  = (Set*) map.get(set);
	map.put(es_ptr, solver->getElementVar(set));
}

void Deserializer::deserializeElementConst(){
	ElementSet* es_ptr;
	myread(&es_ptr, sizeof(ElementSet*));
	VarType type;
	myread(&type, sizeof(VarType));
	uint64_t value;
	myread(&value, sizeof(uint64_t));
	map.put(es_ptr, solver->getElementConst(type, value));
}

void Deserializer::deserializeElementFunction(){
	ElementFunction *ef_ptr;
	myread(&ef_ptr, sizeof(ElementFunction *));
	Function *function;
	myread(&function, sizeof(Function*));
	ASSERT(map.contains(function));
	function = (Function*) map.get(function);
	uint size;
	myread(&size, sizeof(uint));
	Vector<Element*> members;
	for(uint i=0; i<size; i++){
		Element* input;
		myread(&input, sizeof(Element *));
		ASSERT(map.contains(input));
		input = (Element*) map.get(input);
		members.push(input);
	}
	
	Boolean* overflowstatus;
	myread(&overflowstatus, sizeof(Boolean *));
	BooleanEdge tmp(overflowstatus);
	bool isNegated = tmp.isNegated();
	ASSERT(map.contains(tmp.getBoolean()));
	overflowstatus = (Boolean*) map.get(tmp.getBoolean());
	BooleanEdge res(overflowstatus);
	BooleanEdge undefStatus = isNegated?res.negate():res;
	
	map.put(ef_ptr, solver->applyFunction(function, members.expose(), size, undefStatus));
}


void Deserializer::deserializeFunctionOperator(){
	FunctionOperator *fo_ptr;
	myread(&fo_ptr, sizeof(FunctionOperator *));
	ArithOp op;
	myread(&op, sizeof(ArithOp));
	uint size;
	myread(&size, sizeof(uint));
	Vector<Set*> domains;
	for(uint i=0; i<size; i++){
		Set* domain;
		myread(&domain, sizeof(Set*));
		ASSERT(map.contains(domain));
		domain = (Set*) map.get(domain);
		domains.push(domain);
	}
	Set* range;
	myread(&range, sizeof(Set*));
	ASSERT(map.contains(range));
	range = (Set*) map.get(range);
	OverFlowBehavior overflowbehavior;
	myread(&overflowbehavior, sizeof(OverFlowBehavior));
	map.put(fo_ptr, solver->createFunctionOperator(op, domains.expose(), size, range, overflowbehavior));
}

void Deserializer::deserializeFunctionTable(){
	FunctionTable *ft_ptr;
	myread(&ft_ptr, sizeof(FunctionTable *));
	Table* table;
	myread(&table, sizeof(Table*));
	ASSERT(map.contains(table));
	table = (Table*) map.get(table);
	UndefinedBehavior undefinedbehavior;
	myread(&undefinedbehavior, sizeof(UndefinedBehavior));
	
	map.put(ft_ptr, solver->completeTable(table, undefinedbehavior));
}