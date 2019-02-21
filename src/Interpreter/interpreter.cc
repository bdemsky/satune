#include "interpreter.h"
#include <string>
#include "signatureenc.h"
#include "structs.h"
#include "csolver.h"
#include "boolean.h"
#include "predicate.h"
#include "element.h"
#include "signature.h"
#include "set.h"
#include "function.h"
#include "inc_solver.h"
#include <fstream>

using namespace std;

Interpreter::Interpreter(CSolver *_solver): 
	csolver(_solver),
	sigEnc(this)
{
}

Interpreter::~Interpreter(){
}

void Interpreter::encode(){
	dumpHeader();
	SetIteratorBooleanEdge *iterator = csolver->getConstraints();
	Vector<char *> facts;
	while(iterator->hasNext()){
		BooleanEdge constraint = iterator->next();
		string constr = encodeConstraint(constraint);
		char *cstr = new char [constr.length()+1];
		strcpy (cstr, constr.c_str());
		facts.push(cstr);
	}
	dumpAllConstraints(facts);
	for(uint i=0; i< facts.getSize(); i++){
		char *cstr = facts.get(i);
		delete[] cstr;
	}
	delete iterator;
}

uint Interpreter::getTimeout(){
	uint timeout =csolver->getSatSolverTimeout(); 
	return timeout == (uint)NOTIMEOUT? 1000: timeout;
}

int Interpreter::solve(){
	dumpFooter();
	int result = IS_INDETER;
	char buffer [512];
	if( output.is_open()){
		output.close();
	}
	compileRunCommand(buffer, sizeof(buffer));
	int status = system(buffer);
	if (status == 0) {
		//Read data in from results file
		result = getResult();
	}
	return result;
}

string Interpreter::encodeConstraint(BooleanEdge c){
	Boolean *constraint = c.getBoolean();
	string res;
	switch(constraint->type){
		case LOGICOP:{
			res = encodeBooleanLogic((BooleanLogic *) constraint);
			break;
		}
		case PREDICATEOP:{
			res = encodePredicate((BooleanPredicate *) constraint);
			break;
		}
		case BOOLEANVAR:{
			res = encodeBooleanVar( (BooleanVar *) constraint);
			break;
		}
		default:
			ASSERT(0);
	}
	if(c.isNegated()){
		return negateConstraint(res);
	}
	return res;
}

string Interpreter::encodePredicate( BooleanPredicate *bp){
	switch (bp->predicate->type) {
		case TABLEPRED:
			ASSERT(0);
		case OPERATORPRED:
			return encodeOperatorPredicate(bp);
		default:
			ASSERT(0);
	}
}

string Interpreter::encodeOperatorPredicate(BooleanPredicate *constraint){
	PredicateOperator *predicate = (PredicateOperator *) constraint->predicate;
	ASSERT(constraint->inputs.getSize() == 2);
	string str;
	Element *elem0 = constraint->inputs.get(0);
	ASSERT(elem0->type == ELEMSET || elem0->type == ELEMFUNCRETURN || elem0->type == ELEMCONST);
	ValuedSignature *elemSig1 = sigEnc.getElementSignature(elem0);
	if(elem0->type == ELEMFUNCRETURN){
		str += processElementFunction((ElementFunction *) elem0, elemSig1);
	}
	Element *elem1 = constraint->inputs.get(1);
	ASSERT(elem1->type == ELEMSET || elem1->type == ELEMFUNCRETURN || elem1->type == ELEMCONST);
	ValuedSignature *elemSig2 = sigEnc.getElementSignature(elem1);
	if(elem1->type == ELEMFUNCRETURN){
		str += processElementFunction((ElementFunction *) elem1, elemSig2);
	}
	str += operatorPredicateConstraint(predicate->getOp(), elemSig1, elemSig2);
	return str;
}

void Interpreter::writeToFile(string str){
	if(!str.empty()){
		output << str << endl;
	}
}

bool Interpreter::getBooleanValue(Boolean *b){
	return (bool)sigEnc.getValue(b);
}

uint64_t Interpreter::getValue(Element * element){
	return (uint64_t)sigEnc.getValue(element);
}

