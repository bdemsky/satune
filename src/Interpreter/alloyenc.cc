#include "alloyenc.h"
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
#include "cppvector.h"

using namespace std;

#define alloyFileName "satune.als"
#define solutionFile "solution.sol"

AlloyEnc::AlloyEnc(CSolver *_solver): 
	Interpreter(_solver) 
{
	output.open(alloyFileName);
	if(!output.is_open()){
		model_print("AlloyEnc:Error in opening the dump file satune.als\n");
		exit(-1);
	}
}

AlloyEnc::~AlloyEnc(){
	if(output.is_open()){
		output.close();
	}
}

void AlloyEnc::dumpAllConstraints(Vector<char *> &facts){
	output << "fact {" << endl;
	for(uint i=0; i< facts.getSize(); i++){
		char *cstr = facts.get(i);
		writeToFile(cstr);
	}
	output << "}" << endl;
}


int AlloyEnc::getResult(){
	ifstream input(solutionFile, ios::in);
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
				printf( " %s\n", token );
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

void AlloyEnc::dumpFooter(){
	output << "pred show {}" << endl;
	output << "run show for " << sigEnc.getAlloyIntScope() << " int" << endl;
}

void AlloyEnc::dumpHeader(){
	output << "open util/integer" << endl;
}

void AlloyEnc::compileRunCommand(char * command, size_t size){
	snprintf(command, size, "./run.sh timeout %u java -Xmx10000m edu.mit.csail.sdg.alloy4whole.ExampleAlloyCompilerNoViz %s > %s", getTimeout(), alloyFileName, solutionFile);
}

string AlloyEnc::negateConstraint(string constr){
	return "not ( " + constr + " )";
}

string AlloyEnc::encodeBooleanLogic( BooleanLogic *bl){
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

string AlloyEnc::encodeBooleanVar( BooleanVar *bv){
	BooleanSig * boolSig = sigEnc.getBooleanSignature(bv);
	return *boolSig + " = 1";
}

string AlloyEnc::processElementFunction(ElementFunction* elemFunc, ElementSig *signature){
	FunctionOperator *function = (FunctionOperator *) elemFunc->getFunction();
	uint numDomains = elemFunc->inputs.getSize();
	ASSERT(numDomains > 1);
	ElementSig *inputs[numDomains];
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

string AlloyEnc::operatorPredicateConstraint(CompOp op, ElementSig *elemSig1, ElementSig *elemSig2){
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


