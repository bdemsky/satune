import pycsolver as ps
from ctypes import *

SATC_EQUALS = 0

def main():
	csolverlb = ps.loadCSolver()
	solver = csolverlb.createCCSolver()
	s1 = [0, 1, 2]
	set1 = (c_long * len(s1))(*s1)
        s2 = [3, 1, 7]
	set2 = (c_long*len(s2))(*s2)
	s1 = csolverlb.createSet(solver,c_uint(0), set1, c_uint(3))
        s2 = csolverlb.createSet(solver,0, set2, 3)
        e1 = csolverlb.getElementVar(solver,s1)
        e2 = csolverlb.getElementVar(solver,s2)
        d = [s1, s2]
	domain = (c_void_p*len(d))(*d)
        equals = csolverlb.createPredicateOperator(solver,c_uint(SATC_EQUALS), domain, c_uint(2));
        inp = [e1, e2];
	inputs = (c_void_p*len(inp))(*inp)
        b = csolverlb.applyPredicate(solver,equals, inputs, c_uint(2));
        csolverlb.addConstraint(solver,b);
        if csolverlb.solve(solver) == 1:
                print "e1="+ str(csolverlb.getElementValue(solver, e1)) + "\te2=" + str(csolverlb.getElementValue(solver, e2));
        else:
                print "UNSAT";

	csolverlb.deleteCCSolver(solver)


  
if __name__== "__main__":
	main()

