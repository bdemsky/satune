#include "smtinterpreter.h"
#include <string>
#include "signatureenc.h"
#include "structs.h"
#include "csolver.h"
#include "boolean.h"
#include "predicate.h"
#include "element.h"
#include "set.h"
#include "function.h"
#include "inc_solver.h"
#include <fstream>
#include "cppvector.h"
#include "smtsig.h"

using namespace std;

SMTInterpreter::SMTInterpreter(CSolver *_solver): 
	Interpreter(_solver) 
{
	output.open(SMTFILENAME);
	if(!output.is_open()){
		model_print("AlloyEnc:Error in opening the dump file satune.smt\n");
		exit(-1);
	}
}

SMTInterpreter::~SMTInterpreter(){
	if(output.is_open()){
		output.close();
	}
}


ValuedSignature *SMTInterpreter::getBooleanSignature(uint id){
	return new SMTBoolSig(id);
}

ValuedSignature *SMTInterpreter::getElementSignature(uint id, Signature *ssig){
	return new SMTElementSig(id, (SMTSetSig *)ssig);
}

Signature *SMTInterpreter::getSetSignature(uint id, Set *set){
	return new SMTSetSig(id, set);
}

void SMTInterpreter::dumpAllConstraints(Vector<char *> &facts){
	for(uint i=0; i< facts.getSize(); i++){
		char *cstr = facts.get(i);
		writeToFile("(assert " + string(cstr) + ")");
	}
}

void SMTInterpreter::extractValue(char *idline, char *valueline){
	uint id;
	if (1 == sscanf(idline,"%*[^0123456789]%u", &id)){
		char valuestr [512];
		ASSERT(1 == sscanf(valueline,"%s)", valuestr));
		uint value;
		if (strcmp (valuestr, "true)") == 0 ){
			value =1;
		}else if (strcmp(valuestr, "false)") == 0){
			value = 0;
		}else {
			ASSERT(1 == sscanf(valueline, "%*[^0123456789]%u", &value));
		}

		model_print("Signature%u = %u\n", id, value);
		sigEnc.setValue(id, value);
	} else {
		ASSERT(0);
	}
}

int SMTInterpreter::getResult(){
	ifstream input(SMTSOLUTIONFILE, ios::in);
	string line;
	while(getline(input, line)){
		if(line.find("unsat")!= line.npos){
			return IS_UNSAT;
		}
		if(line.find("(define-fun") != line.npos || line.find("( (") != line.npos){
			string valueline;
			ASSERT(getline(input, valueline));
			char cline [line.size()+1];
			strcpy ( cline, line.c_str() );
			char vline [valueline.size()+1];
			strcpy ( vline, valueline.c_str() );
			extractValue(cline, vline);
		}
	}
	return IS_SAT;
}

void SMTInterpreter::dumpFooter(){
	output << "(check-sat)" << endl;
	output << "(get-model)" << endl;
}

void SMTInterpreter::dumpHeader(){
}

void SMTInterpreter::compileRunCommand(char * command, size_t size){
	model_print("Calling Z3...\n");
	snprintf(command, size, "./run.sh z3 -T:%u -in < %s > %s", getTimeout(), SMTFILENAME, SMTSOLUTIONFILE);
}

string SMTInterpreter::negateConstraint(string constr){
	return "( not " + constr + " )";
}

string SMTInterpreter::encodeBooleanLogic( BooleanLogic *bl){
	uint size = bl->inputs.getSize();
	string array[size];
	for (uint i = 0; i < size; i++)
		array[i] = encodeConstraint(bl->inputs.get(i));
	string op;
	switch (bl->op){
		case SATC_OR:
			op = "or";
			break;
		case SATC_AND:
			op = "and";
			break;
		case SATC_NOT:
			op = "not";
			break;
		case SATC_IMPLIES:
			op = "=>";
			break;
		case SATC_XOR:
		default:
			ASSERT(0);
	}
	switch (bl->op) {
		case SATC_XOR:
		case SATC_OR:
		case SATC_AND:{
			ASSERT(size >= 2);
			string res = array[0];
			for( uint i=1; i< size; i++){
				res = "( " + op + " " + res + " " +  array[i] + " )";
			}
			return res;
		}
		case SATC_NOT:{
			return "( not " + array[0] + " )";
		}
		case SATC_IMPLIES:
			return "( " + op + " " + array[0] + " " + array[1] + " )";
		case SATC_IFF:
		default:
			ASSERT(0);

	}
}

string SMTInterpreter::encodeBooleanVar( BooleanVar *bv){
	ValuedSignature * boolSig = sigEnc.getBooleanSignature(bv);
	return *boolSig + "";
}

string SMTInterpreter::processElementFunction(ElementFunction* elemFunc, ValuedSignature *signature){
	FunctionOperator *function = (FunctionOperator *) elemFunc->getFunction();
	uint numDomains = elemFunc->inputs.getSize();
	ASSERT(numDomains > 1);
	ValuedSignature *inputs[numDomains];
	string result;
	for (uint i = 0; i < numDomains; i++) {
		Element *elem = elemFunc->inputs.get(i);
		inputs[i] = sigEnc.getElementSignature(elem);
		if(elem->type == ELEMFUNCRETURN){
			result += processElementFunction((ElementFunction *) elem, inputs[i]);
		}
	}
	string op;
	switch(function->op){
		case SATC_ADD:
			op = "+";
			break;
		case SATC_SUB:
			op = "-";
			break;
		default:
			ASSERT(0);
	}
	result += "( = " + *signature; 
	string tmp = "" + *inputs[0];
	for (uint i = 1; i < numDomains; i++) {
		tmp = "( " + op + " " + tmp + " " + *inputs[i] + " )";
	}
	result += tmp + " )";
	return result;
}

string SMTInterpreter::operatorPredicateConstraint(CompOp op, ValuedSignature *elemSig1, ValuedSignature *elemSig2){
	switch (op) {
		case SATC_EQUALS:
			return "( = " + *elemSig1 + " " + *elemSig2 +" )";
		case SATC_LT:
			return "( < " + *elemSig1 + " " + *elemSig2 + " )";
		case SATC_GT:
			return "(> " + *elemSig1 + " " + *elemSig2 + " )";
		default:
			ASSERT(0);
	}
}


