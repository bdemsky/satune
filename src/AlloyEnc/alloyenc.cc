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
#include <fstream>
#include <regex>

using namespace std;

const char * AlloyEnc::alloyFileName = "satune.als";
const char * AlloyEnc::solutionFile = "solution.sol";

AlloyEnc::AlloyEnc(CSolver *_solver): 
	csolver(_solver),
	sigEnc(this)
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

void AlloyEnc::encode(){
	dumpAlloyHeader();
	SetIteratorBooleanEdge *iterator = csolver->getConstraints();
	Vector<char *> facts;
	while(iterator->hasNext()){
		BooleanEdge constraint = iterator->next();
		string constr = encodeConstraint(constraint);
		//model_print("constr=%s\n", constr.c_str());
		char *cstr = new char [constr.length()+1];
		strcpy (cstr, constr.c_str());
		facts.push(cstr);
	}
	output << "fact {" << endl;
	for(uint i=0; i< facts.getSize(); i++){
		char *cstr = facts.get(i);
		writeToFile(cstr);
		delete[] cstr;
	}
	output << "}" << endl;
	delete iterator;
}

int AlloyEnc::getResult(){
	ifstream input(solutionFile, ios::in);
	string line;
	while(getline(input, line)){
		if(regex_match(line, regex("Unsatisfiable."))){
			return IS_UNSAT;
		}
		if(regex_match(line, regex(".*Element\\d+.*value=.*Element\\d+.*->\\d+.*"))){
			int tmp=0, index=0, value=0;
			const char* s = line.c_str();
			uint i1, i2, i3;
			uint64_t i4;
			if (4 == sscanf(s,"%*[^0123456789]%u%*[^0123456789]%d%*[^0123456789]%d%*[^0123456789]%" PRId64 "", &i1, &i2, &i3, &i4)){
				model_print("Element%d = %" PRId64 "\n", i1, i4);
				sigEnc.setValue(i1, i4);
			}
		}
	}
	return IS_SAT;
}

void AlloyEnc::dumpAlloyFooter(){
	output << "pred show {}" << endl;
	output << "run show for " << sigEnc.getAlloyIntScope() << " int" << endl;
}

void AlloyEnc::dumpAlloyHeader(){
	output << "open util/integer" << endl;
}

int AlloyEnc::solve(){
	dumpAlloyFooter();
	int result = IS_INDETER;
	char buffer [512];
	if( output.is_open()){
		output.close();
	}
	snprintf(buffer, sizeof(buffer), "./run.sh java -Xmx10000m edu.mit.csail.sdg.alloy4whole.ExampleAlloyCompilerNoViz %s > %s", alloyFileName, solutionFile);
	int status = system(buffer);
	if (status == 0) {
		//Read data in from results file
		result = getResult();
	}
	return result;
}

string AlloyEnc::encodeConstraint(BooleanEdge c){
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
		return "not ( " + res + " )";
	}
	return res;
}

string AlloyEnc::encodeBooleanLogic( BooleanLogic *bl){
	uint size = bl->inputs.getSize();
	string array[size];
	for (uint i = 0; i < size; i++)
		array[i] = encodeConstraint(bl->inputs.get(i));
	switch (bl->op) {
		case SATC_AND:{
			ASSERT(size >= 2);
			string res = "";
			res += array[0];
			for( uint i=1; i< size; i++){
				res += " and " + array[i];
			}
			return res;
		}
		case SATC_NOT:{
			return "not " + array[0];
		}
		case SATC_IFF:
			return array[0] + " iff " + array[1];
		case SATC_OR:
		case SATC_XOR:
		case SATC_IMPLIES:
		default:
			ASSERT(0);

	}
}

string AlloyEnc::encodePredicate( BooleanPredicate *bp){
	switch (bp->predicate->type) {
		case TABLEPRED:
			ASSERT(0);
		case OPERATORPRED:
			return encodeOperatorPredicate(bp);
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

string AlloyEnc::encodeOperatorPredicate(BooleanPredicate *constraint){
	constraint->print();
	PredicateOperator *predicate = (PredicateOperator *) constraint->predicate;
	ASSERT(constraint->inputs.getSize() == 2);
	string str;
	Element *elem0 = constraint->inputs.get(0);
	ASSERT(elem0->type == ELEMSET || elem0->type == ELEMFUNCRETURN || elem0->type == ELEMCONST);
	ElementSig *elemSig1 = sigEnc.getElementSignature(elem0);
	if(elem0->type == ELEMFUNCRETURN){
		str += processElementFunction((ElementFunction *) elem0, elemSig1);
	}
	Element *elem1 = constraint->inputs.get(1);
	ASSERT(elem1->type == ELEMSET || elem1->type == ELEMFUNCRETURN || elem1->type == ELEMCONST);
	ElementSig *elemSig2 = sigEnc.getElementSignature(elem1);
	if(elem1->type == ELEMFUNCRETURN){
		str += processElementFunction((ElementFunction *) elem1, elemSig2);
	}
	switch (predicate->getOp()) {
		case SATC_EQUALS:
			str += *elemSig1 + " = " + *elemSig2;
			break;
		case SATC_LT:
			str += *elemSig1 + " < " + *elemSig2;
			break;
		case SATC_GT:
			str += *elemSig1 + " > " + *elemSig2;
			break; 
		default:
			ASSERT(0);
	}
	return str;
}

void AlloyEnc::writeToFile(string str){
	output << str << endl;
}

bool AlloyEnc::getBooleanValue(Boolean *b){
	if (b->boolVal == BV_MUSTBETRUE)
		return true;
	else if (b->boolVal == BV_MUSTBEFALSE)
		return false;
	return sigEnc.getBooleanSignature(b);
}

uint64_t AlloyEnc::getValue(Element * element){
	ElementEncoding *elemEnc = element->getElementEncoding();
	if (elemEnc->numVars == 0)//case when the set has only one item
		return element->getRange()->getElement(0);
	return sigEnc.getValue(element);
}

