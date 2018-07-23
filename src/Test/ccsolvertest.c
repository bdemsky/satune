#include "ccsolver.h"
#include <stdio.h>

#define SATC_EQUALS 0

int main (int num, char** args){
	void* solver = createCCSolver();
	long set1[] = {0, 1, 2};
        long set2[] = {3, 1, 7};
        void *s1 = createSet(solver,0, set1, 3);
        void *s2 = createSet(solver,0, set2, 3);
        void *e1 = getElementVar(solver,s1);
        void *e2 = getElementVar(solver,s2);
        void *equals = createPredicateOperator(solver,SATC_EQUALS);
        void *inputs[] = {e1, e2};
        void* b = applyPredicate(solver,equals, inputs, 2);
        addConstraint(solver,b);
        if (solve(solver) == 1)
                printf("e1=%ld \t e2=%ld\n", getElementValue(solver, e1), getElementValue(solver, e2));
        else
                printf("UNSAT\n");
        deleteCCSolver(solver);
}

