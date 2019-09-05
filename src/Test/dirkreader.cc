#include <iostream>
#include <fstream>
#include "csolver.h"
#include "hashset.h"
#include "cppvector.h"

const char * assertstart = "ASSERTSTART";
const char * assertend = "ASSERTEND";
const char * satstart = "SATSTART";
const char * satend = "SATEND";

int skipahead(const char * token1, const char * token2);
char * readuntilend(const char * token);
BooleanEdge parseConstraint(CSolver * solver, char * constraint, int & offset);
void processEquality(char * constraint, int &offset);

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
      }
      assert = readuntilend(assertend);
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
      delete solver;
    }
  }

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

void skipwhitespace(char * constraint, int & offset) {
  //skip whitespace
  while (constraint[offset] == ' ' || constraint[offset] == '\n')
    offset++;
}

void doExit() {
  printf("PARSE ERROR\n");
  exit(-1);
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
    if (constraint[offset++] != ')')
      doExit();
    return;
  } else if (strncmp(&constraint[offset], "(=", 2) == 0) {
    offset+=2;
    skipwhitespace(constraint, offset);
    int v1=getOrderVar(constraint, offset);
    skipwhitespace(constraint, offset);
    int v2=getOrderVar(constraint, offset);
    skipwhitespace(constraint, offset);
    if (constraint[offset++] != ')')
      doExit();
    mergeVars(v1, v2);
    return;
  } else {
    getBooleanVar(constraint, offset);
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
    skipwhitespace(constraint, offset);
    int32_t v2 = getOrderVar(constraint, offset);
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
  } else {
    int32_t v = getBooleanVar(constraint, offset);
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
