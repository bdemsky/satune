
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
	Order* optr;
	myread(&optr, sizeof(Order*));
	uint64_t first;
	myread(&first, sizeof(uint64_t));
	uint64_t second;
	myread(&second, sizeof(uint64_t));
	ASSERT(map.contains(optr));
	Order* order  = (Order*) map.get(optr);
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
	uint64_t low;
	myread(&low, sizeof(uint64_t));
	uint64_t high;
	myread(&high, sizeof(uint64_t));
	uint size;
	myread(&size, sizeof(uint));
	Vector<uint64_t> members;
	for(uint i=0; i<size; i++){
		uint64_t mem;
		myread(&mem, sizeof(uint64_t));
		members.push(mem);
	}
	Set *set = isRange? solver->createRangeSet(type, low, high):
		solver->createSet(type, members.expose(), size);
	map.put(s_ptr, set);
}
