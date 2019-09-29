#include <iostream>
#include <fstream>
#include "csolver.h"
#include "hashset.h"
#include "cppvector.h"
#include <dirent.h>
#include <string>
#include <sstream>
#include "common.h"
using namespace std;
const char * assertstart = "ASSERTSTART";
const char * assertend = "ASSERTEND";
const char * satstart = "SATSTART";
const char * satend = "SATEND";

int skipahead(const char * token1, const char * token2);
char * readuntilend(const char * token);
BooleanEdge parseConstraint(CSolver * solver, char * constraint, int & offset);
void processEquality(char * constraint, int &offset);
void dumpSMTproblem(string &smtProblem, long long id);
void renameDumpFile(const char *path, long long id);
void addASSERTConstraintToSMTProblem(string & smtProblem, char * assert);
void addSATConstraintToSMTProblem(string &smtProblem, char * sat);
void createSATtuneDumps (char *assert, char *sat, long long &smtID);

std::ifstream * file;
Order * order;
MutableSet * set;
class intwrapper {
public:
  intwrapper(int _val) : value(_val) {
  }
  int32_t value;
};

unsigned int iw_hash_function(intwrapper *i) {
  return i->value;
}

bool iw_equals(intwrapper *key1, intwrapper *key2) {
  return (key1->value == key2->value);
}

typedef Hashset<intwrapper *, uintptr_t, PTRSHIFT, iw_hash_function, iw_equals> HashsetIW;
typedef SetIterator<intwrapper *, uintptr_t, PTRSHIFT, iw_hash_function, iw_equals> HashsetIWIterator;
class OrderRec {
public:
  OrderRec() : set (NULL), value(-1), alloced(false) {
  }
  ~OrderRec() {
    if (set != NULL) {
      set->resetAndDelete();
      delete set;
    }
  }
  HashsetIW * set;
  int32_t value;
  bool alloced;
};

typedef Hashtable<intwrapper *, OrderRec *, uintptr_t, PTRSHIFT, iw_hash_function, iw_equals> HashtableIW;
typedef Hashtable<intwrapper *, Boolean *, uintptr_t, PTRSHIFT, iw_hash_function, iw_equals> HashtableBV;
Vector<OrderRec*> * orderRecVector = NULL;
HashtableIW * ordertable = NULL;
HashtableBV * vartable = new HashtableBV();
HashsetIW * intSet = new HashsetIW();
HashsetIW * boolSet = new HashsetIW();

void cleanAndFreeOrderRecVector(){
	for(uint i=0; i< orderRecVector->getSize(); i++){
		delete orderRecVector->get(i);
	}
	orderRecVector->clear();
}

int main(int numargs, char ** argv) {
  file = new std::ifstream(argv[1]);
  char * assert = NULL;
  char * sat = NULL;
  string smtProblem = "";
  long long smtID=0L;
  Vector<OrderRec*> orderRecs;
  orderRecVector = &orderRecs;
  while(true) {
    int retval = skipahead(assertstart, satstart);
    printf("%d\n", retval);
    if (retval == 0){
      break;
    }
    if (retval == 1) {
      if (assert != NULL){
	free(assert);
	assert = NULL;
	dumpSMTproblem(smtProblem, smtID);
	smtProblem = "";
	intSet->reset();
	boolSet->reset();
      }
      assert = readuntilend(assertend);
      addASSERTConstraintToSMTProblem(smtProblem, assert);
      //cout << "INFO: SMTProblem After adding ASSERT Constraints\n" << smtProblem << "\n&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n";
      printf("[%s]\n", assert);
    } else if (retval == 2) {
      if (sat != NULL) {
	free(sat);
	sat = NULL;
	vartable->resetAndDeleteKeys();
	ordertable->reset();
	cleanAndFreeOrderRecVector();
      } else {
	ordertable = new HashtableIW();
      }
      sat = readuntilend(satend);
      createSATtuneDumps(assert, sat, smtID);
      addSATConstraintToSMTProblem(smtProblem, sat);
      //cout << "INFO: SMTProblem After adding SAT Constraints\n" << smtProblem << "\n&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n";
    }
  }
  
  if(smtID == 0L){
  	char trueString [] = "true";
	ordertable = new HashtableIW();
  	createSATtuneDumps(assert, trueString, smtID);
	addSATConstraintToSMTProblem(smtProblem, trueString);
  }
  dumpSMTproblem(smtProblem, smtID);
  cleanAndFreeOrderRecVector();
  if(ordertable != NULL){
  	delete ordertable;
  }
  if(assert != NULL){
  	free(assert);
  }
  if(sat != NULL){
  	free(sat);
  }
  vartable->resetAndDeleteKeys();
  delete vartable;
  file->close();
  delete file;
}

void createSATtuneDumps (char *assert, char *sat, long long &smtID){
	CSolver *solver = new CSolver();
	solver->turnoffOptimizations();
	set = solver->createMutableSet(1);
	order = solver->createOrder(SATC_TOTAL, (Set *) set);
	int offset = 0;
	processEquality(sat, offset);
	offset = 0;
	processEquality(assert, offset);
	offset = 0;
	solver->addConstraint(parseConstraint(solver, sat, offset));
	offset = 0;
	solver->addConstraint(parseConstraint(solver, assert, offset));
	printf("[%s]\n", sat);
	solver->finalizeMutableSet(set);
	solver->serialize();
	solver->printConstraints();
	smtID= getTimeNano();
	renameDumpFile("./", smtID);
      	delete solver;
}

void doExit(const char * message){
	printf("%s", message);
	exit(-1);
}

void doExit() {
	doExit("Parse Error\n");
}

void renameDumpFile(const char *path, long long id) {
	struct dirent *entry;
	DIR *dir = opendir(path);
	if (dir == NULL) {
		return;
	}
	char newname[128] ={0};
	sprintf(newname, "%lld.dump", id);
	bool duplicate = false;
	while ((entry = readdir(dir)) != NULL) {
		 if(strstr(entry->d_name, "DUMP") != NULL){
		 	if(duplicate){
				doExit("Multiplle DUMP file exists. Make sure to clean them all before running the program");
			}
		 	int result= rename( entry->d_name , newname );
			if ( result == 0 )
				printf ( "File successfully renamed to %s\n" , newname);
			else
				doExit( "Error renaming file" );
			duplicate = true;
		 }
	}
	closedir(dir);
}
template <class MyType>
string to_string(MyType value){
	string number;
	std::stringstream strstream;
	strstream << value;
	strstream >> number;
	return number;
}

void dumpDeclarations(std::ofstream& smtfile){
	HashsetIWIterator* iterator = intSet->iterator();
	smtfile << "(set-logic QF_LRA)" << endl;
	while(iterator->hasNext()){
		intwrapper * iw = iterator->next();
		string declare = "(declare-const O!"+ to_string<uint32_t>(iw->value) + " Real)\n";
		smtfile << declare;
	}
	delete iterator;

	iterator = boolSet->iterator();
        while(iterator->hasNext()){
                intwrapper * iw = iterator->next();
                string declare = "(declare-const S!" + to_string<uint32_t>(iw->value) + " Bool)\n";
                smtfile << declare;
        }
        delete iterator;
}

void addASSERTConstraintToSMTProblem(string & smtProblem, char * assert){
	string ssat(assert);
	string assertStatement = "(assert "+ ssat + "\n)\n";
	smtProblem += assertStatement;
}

void addSATConstraintToSMTProblem(string &smtProblem, char * sat){
	string ssat(sat);
        string satStatement = "(push)\n(assert "+ ssat + "\n)\n(check-sat)\n(get-model)\n(pop)\n";
        smtProblem += satStatement;
}


void dumpSMTproblem(string &smtProblem, long long id){
	std::ofstream smtfile;
	string smtfilename = to_string<long long>(id) + ".smt";
        smtfile.open (smtfilename.c_str());
        if(!smtfile.is_open())
        {
                doExit("Unable to create file.\n");
        }
	dumpDeclarations(smtfile);
	smtfile << smtProblem;
	smtfile.close();
	cout <<"************here is the SMT file*********" << endl << smtProblem  <<endl << "****************************" << endl;;
}


void skipwhitespace(char * constraint, int & offset) {
  //skip whitespace
  while (constraint[offset] == ' ' || constraint[offset] == '\n')
    offset++;
}

int32_t getOrderVar(char *constraint, int & offset) {
  if (constraint[offset++] != 'O' || constraint[offset++] != '!' ) {
    doExit();
  }
  int32_t num = 0;
  while(true) {
    char next = constraint[offset];
    if (next >= '0' && next <= '9') {
      num = (num * 10) + (next - '0');
      offset++;
    } else
      return num;
  }
}

int32_t getBooleanVar(char *constraint, int & offset) {
	if (constraint[offset++] != 'S' || constraint[offset++] != '!' ) {
		cout << "Constraint= " << constraint << "\toffset=" << constraint[offset - 1] << " ******\n";
		doExit();
	}
  int32_t num = 0;
  while(true) {
    char next = constraint[offset];
    if (next >= '0' && next <= '9') {
      num = (num * 10) + (next - '0');
      offset++;
    } else
      return num;
  }
}

BooleanEdge checkBooleanVar(CSolver *solver, int32_t value) {
  intwrapper v(value);
  if (vartable->contains(&v)) {
    return BooleanEdge(vartable->get(&v));
  } else {
    Boolean* ve = solver->getBooleanVar(2).getBoolean();
    vartable->put(new intwrapper(value), ve);
    return BooleanEdge(ve);
  }
}

void mergeVars(int32_t value1, int32_t value2) {
  intwrapper v1(value1);
  intwrapper v2(value2);
  OrderRec *r1 = ordertable->get(&v1);
  OrderRec *r2 = ordertable->get(&v2);
  if (r1 == r2) {
    if (r1 == NULL) {
      OrderRec * r = new OrderRec();
      orderRecVector->push(r);
      r->value = value1;
      r->set = new HashsetIW();
      intwrapper * k1 = new intwrapper(v1);
      intwrapper * k2 = new intwrapper(v2);
      r->set->add(k1);
      r->set->add(k2);
      ordertable->put(k1, r);
      ordertable->put(k2, r);
    }
  } else if (r1 == NULL) {
    intwrapper * k = new intwrapper(v1);
    ordertable->put(k, r2);
    r2->set->add(k);
  } else if (r2 == NULL) {
    intwrapper * k = new intwrapper(v2);
    ordertable->put(k, r1);
    r1->set->add(k);
  } else {
    SetIterator<intwrapper *, uintptr_t, PTRSHIFT, iw_hash_function, iw_equals> * it1 = r1->set->iterator();
    while (it1->hasNext()) {
      intwrapper * k = it1->next();
      ordertable->put(k, r2);
      r2->set->add(k);
    }
    delete r2->set;
    r2->set = NULL;
    delete r2;
    delete it1;
  }
}

int32_t checkordervar(CSolver * solver, int32_t value) {
  intwrapper v(value);
  OrderRec * rec = ordertable->get(&v);
  if (rec == NULL) {
    intwrapper * k = new intwrapper(value);
    rec = new OrderRec();
    orderRecVector->push(rec);
    rec->value = value;
    rec->set = new HashsetIW();
    rec->set->add(k);
    ordertable->put(k, rec);
  }
  if (!rec->alloced) {
    solver->addItem(set, rec->value);
    rec->alloced = true;
  }
  return rec->value;
}

void processEquality(char * constraint, int &offset) {
  skipwhitespace(constraint, offset);
  if (strncmp(&constraint[offset], "(and",4) == 0) {
    offset +=4;
    Vector<BooleanEdge> vec;
    while(true) {
      skipwhitespace(constraint, offset);
      if (constraint[offset] == ')') {
	offset++;
	return;
      }
      processEquality(constraint, offset);
    }
  } else if (strncmp(&constraint[offset], "(<", 2) == 0) {
    offset+=2;
    skipwhitespace(constraint, offset);
    getOrderVar(constraint, offset);
    skipwhitespace(constraint, offset);
    getOrderVar(constraint, offset);
    skipwhitespace(constraint, offset);
    if (constraint[offset++] != ')'){
	    doExit();
    }
    return;
  } else if (strncmp(&constraint[offset], "(=", 2) == 0) {
    offset+=2;
    skipwhitespace(constraint, offset);
    int v1=getOrderVar(constraint, offset);
    intwrapper iwv1(v1);
    if(intSet->get(&iwv1) == NULL){
	intSet->add(new intwrapper(v1));
    }
    skipwhitespace(constraint, offset);
    int v2=getOrderVar(constraint, offset);
    intwrapper iwv2(v2);
    if(intSet->get(&iwv2) == NULL){
	intSet->add(new intwrapper(v2));
    }
    skipwhitespace(constraint, offset);
    if (constraint[offset++] != ')')
      doExit();
    mergeVars(v1, v2);
    return;
  } else if (strncmp(&constraint[offset], "tr", 2) == 0){
	offset+=4;
	skipwhitespace(constraint, offset);
	return;
  }
  else{
	int32_t b1 = getBooleanVar(constraint, offset);
	intwrapper iwb1(b1);
	if(boolSet->get(&iwb1) == NULL){
	    boolSet->add(new intwrapper(b1));
	}
	skipwhitespace(constraint, offset);
return;
  }
}

BooleanEdge parseConstraint(CSolver * solver, char * constraint, int & offset) {
  skipwhitespace(constraint, offset);
  if (strncmp(&constraint[offset], "(and", 4) == 0) {
    offset +=4;
    Vector<BooleanEdge> vec;
    while(true) {
      skipwhitespace(constraint, offset);
      if (constraint[offset] == ')') {
	offset++;
	return solver->applyLogicalOperation(SATC_AND, vec.expose(), vec.getSize());
      }
      vec.push(parseConstraint(solver, constraint, offset));
    }
  } else if (strncmp(&constraint[offset], "(<", 2) == 0) {
    offset+=2;
    skipwhitespace(constraint, offset);
    int32_t v1 = getOrderVar(constraint, offset);
    intwrapper iwv1(v1);
    if(intSet->get(&iwv1) == NULL){
        intSet->add(new intwrapper(v1));
    }
    skipwhitespace(constraint, offset);
    int32_t v2 = getOrderVar(constraint, offset);
    intwrapper iwv2(v2);
    if(intSet->get(&iwv2) == NULL){
        intSet->add(new intwrapper(v2));
    }
    skipwhitespace(constraint, offset);
    if (constraint[offset++] != ')')
      doExit();
    return solver->orderConstraint(order, checkordervar(solver, v1), checkordervar(solver, v2));
  } else if (strncmp(&constraint[offset], "(=", 2) == 0) {
    offset+=2;
    skipwhitespace(constraint, offset);
    getOrderVar(constraint, offset);
    skipwhitespace(constraint, offset);
    getOrderVar(constraint, offset);
    skipwhitespace(constraint, offset);
    if (constraint[offset++] != ')')
      doExit();
    return solver->getBooleanTrue();
  }else if (strncmp(&constraint[offset], "tr", 2) == 0){
	offset+=4;
	skipwhitespace(constraint, offset);
	return solver->getBooleanTrue();
  }
  else {
    int32_t v = getBooleanVar(constraint, offset);
    intwrapper iwb1(v);
    if(boolSet->get(&iwb1) == NULL){
        boolSet->add(new intwrapper(v));
    }
    skipwhitespace(constraint, offset);
    return checkBooleanVar(solver, v);
  }
}

int skipahead(const char * token1, const char * token2) {
  int index1 = 0, index2 = 0;
  char buffer[256];
  while(true) {
    if (token1[index1] == 0)
      return 1;
    if (token2[index2] == 0)
      return 2;
    if (file->eof())
      return 0;
    file->read(buffer, 1);
    if (buffer[0] == token1[index1])
      index1++;
    else
      index1 = 0;
    if (buffer[0] == token2[index2])
      index2++;
    else
      index2 = 0;
  }
}

char * readuntilend(const char * token) {
  int index = 0;
  char buffer[256];
  int length = 256;
  int offset = 0;
  char * outbuffer = (char *) malloc(length);
  while(true) {
    if (token[index] == 0) {
      outbuffer[offset - index] = 0;
      return outbuffer;
    }
    if (file->eof()) {
      free(outbuffer);
      return NULL;
    }
      
    file->read(buffer, 1);
    outbuffer[offset++] = buffer[0];
    if (offset == length) {
      length = length * 2;
      outbuffer = (char *) realloc(outbuffer, length);
    }
    if (buffer[0] == token[index])
      index++;
    else
      index=0;
  }
}
