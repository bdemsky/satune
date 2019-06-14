#include "csolver.h"

#define INPUTSIZE 2
#define DOMAINSIZE 3

int main(int numargs, char **argv) {
	CSolver *solver = new CSolver();
	uint64_t set1[] = {3, 1, 2};
	uint64_t set2[] = {3, 1, 7};
	Set *s1 = solver->createSet(0, set1, DOMAINSIZE);
	Set *s2 = solver->createSet(0, set2, DOMAINSIZE);
	Element *e1 = solver->getElementVar(s1);
	Element *e2 = solver->getElementVar(s2);
	Predicate *equals = solver->createPredicateOperator(SATC_EQUALS);
	Element *inputs[] = {e1, e2};
	BooleanEdge b = solver->applyPredicate(equals, inputs, INPUTSIZE);
	solver->addConstraint(b);
	solver->freezeElement(e1);
	solver->freezeElement(e2);
	if (solver->solve() == 1){
		int run = 1;
		do{
			printf("result %d: e1=%" PRIu64 " e2=%" PRIu64 "\n", run, solver->getElementValue(e1), solver->getElementValue(e2));
			for(int i=0; i< INPUTSIZE; i++){
				uint64_t val = solver->getElementValue(inputs[i]);
				Element *econst = solver->getElementConst(0, val);
				Element * tmpInputs[] = {inputs[i], econst};
				BooleanEdge b = solver->applyPredicate(equals, tmpInputs, INPUTSIZE);
				solver->addConstraint(solver->applyLogicalOperation(SATC_NOT, b));
			}
			run++;
		}while(solver->solveIncremental() == 1);
		printf("After %d runs, there are no more models to find ...\n", run);
	}else{
		printf("UNSAT\n");
	}
	delete solver;
}
