
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
	ssize_t t = read (filedesc, __buf, __nbytes);
	write (1, __buf, __nbytes);
	model_print("read\n");
	return t;
}

CSolver * Deserializer::deserialize(){
	ASTNodeType nodeType;
	while(myread(&nodeType, sizeof(ASTNodeType) ) >0){
		switch(nodeType){
			case BOOLEANEDGE:
				deserializeBooleanEdge();
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
