#include "alloyinterpreter.h"
#include <string>
#include "signatureenc.h"
#include "structs.h"
#include "csolver.h"
#include "boolean.h"
#include "predicate.h"
#include "element.h"
#include "alloysig.h"
#include "set.h"
#include "function.h"
#include "inc_solver.h"
#include <fstream>
#include "cppvector.h"
#include "math.h"

using namespace std;

#define ALLOYFILENAME "satune.als"
#define ALLOYSOLUTIONFILE "solution.sol"

AlloyInterpreter::AlloyInterpreter(CSolver *_solver): 
	Interpreter(_solver) 
{
	output.open(ALLOYFILENAME);
	if(!output.is_open()){
		model_print("AlloyEnc:Error in opening the dump file satune.als\n");
		exit(-1);
	}
}

AlloyInterpreter::~AlloyInterpreter(){
	if(output.is_open()){
		output.close();
	}
}

ValuedSignature *AlloyInterpreter::getBooleanSignature(uint id){
	return new AlloyBoolSig(id);
}

ValuedSignature *AlloyInterpreter::getElementSignature(uint id, Signature *ssig){
	return new AlloyElementSig(id, ssig);
}

Signature *AlloyInterpreter::getSetSignature(uint id, Set *set){
	return new AlloySetSig(id, set);
}

void AlloyInterpreter::dumpAllConstraints(Vector<char *> &facts){
	output << "fact {" << endl;
	for(uint i=0; i< facts.getSize(); i++){
		char *cstr = facts.get(i);
		writeToFile(cstr);
	}
	output << "}" << endl;
}


int AlloyInterpreter::getResult(){
	ifstream input(ALLOYSOLUTIONFILE, ios::in);
	string line;
	while(getline(input, line)){
		if(line.find("Unsatisfiable.")== 0){
			return IS_UNSAT;
		}
		if(line.find("univ") == 0){
			continue;
		}
		if(line.find("this/AbsBool<:value") == 0 || line.find("this/AbsElement<:value=") == 0){
			const char delim [2] = ",";
			char cline [line.size()+1];
			strcpy ( cline, line.c_str() );
			char *token = strtok(cline, delim);
			while( token != NULL ) {
				uint i1, i2, i3;
				if (3 == sscanf(token,"%*[^0123456789]%u%*[^0123456789]%d%*[^0123456789]%u", &i1, &i2, &i3)){
					model_print("Signature%u = %u\n", i1, i3);
					sigEnc.setValue(i1, i3);
				}
				token = strtok(NULL, delim);
			}
		}
	}
	return IS_SAT;
}


int AlloyInterpreter::getAlloyIntScope(){
	double mylog = log2(sigEnc.getMaxValue() + 1);
	return floor(mylog) == mylog ? (int)mylog + 1: (int)mylog + 2;
}

void AlloyInterpreter::dumpFooter(){
	output << "pred show {}" << endl;
	output << "run show for " << getAlloyIntScope() << " int" << endl;
}

void AlloyInterpreter::dumpHeader(){
	output << "open util/integer" << endl;
}

void AlloyInterpreter::compileRunCommand(char * command, size_t size){
	model_print("Calling Alloy...\n");
	snprintf(command, size, "./run.sh timeout %u java -Xmx10000m edu.mit.csail.sdg.alloy4whole.ExampleAlloyCompilerNoViz %s > %s", getTimeout(), ALLOYFILENAME, ALLOYSOLUTIONFILE);
}

string AlloyInterpreter::negateConstraint(string constr){
	return "not ( " + constr + " )";
}

string AlloyInterpreter::encodeBooleanLogic( BooleanLogic *bl){
	uint size = bl->inputs.getSize();
	string array[size];
	for (uint i = 0; i < size; i++)
		array[i] = encodeConstraint(bl->inputs.get(i));
	string op;
	switch (bl->op){
		case SATC_OR:
			op = " or ";
			break;
		case SATC_AND:
			op = " and ";
			break;
		case SATC_NOT:
			op = " not ";
			break;
		case SATC_IFF:
			op = " iff ";
			break;
		case SATC_IMPLIES:
			op = " implies ";
			break;
		case SATC_XOR:
		default:
			ASSERT(0);
	}
	switch (bl->op) {
		case SATC_OR:
		case SATC_AND:{
			ASSERT(size >= 2);
			string res = "( ";
			res += array[0];
			for( uint i=1; i< size; i++){
				res += op + array[i];
			}
			res += " )";
			return res;
		}
		case SATC_NOT:{
			return "not ( " + array[0] + " )";
		}
		case SATC_IMPLIES:
		case SATC_IFF:
			return "( " + array[0] + op + array[1] + " )";
		case SATC_XOR:
		default:
			ASSERT(0);

	}
}

string AlloyInterpreter::encodeBooleanVar( BooleanVar *bv){
	ValuedSignature * boolSig = sigEnc.getBooleanSignature(bv);
	return *boolSig + " = 1";
}

string AlloyInterpreter::processElementFunction(ElementFunction* elemFunc, ValuedSignature *signature){
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
			op = ".add";
			break;
		case SATC_SUB:
			op = ".sub";
			break;
		default:
			ASSERT(0);
	}
	result += *signature + " = " + *inputs[0];
	for (uint i = 1; i < numDomains; i++) {
		result += op + "["+ *inputs[i] +"]";
	}
	result += "\n";
	return result;
}

string AlloyInterpreter::operatorPredicateConstraint(CompOp op, ValuedSignature *elemSig1, ValuedSignature *elemSig2){
	switch (op) {
		case SATC_EQUALS:
			return *elemSig1 + " = " + *elemSig2;
		case SATC_LT:
			return *elemSig1 + " < " + *elemSig2;
		case SATC_GT:
			return *elemSig1 + " > " + *elemSig2;
		default:
			ASSERT(0);
	}
}


