from ctypes import *

def loadCSolver():
        csolverlb = cdll.LoadLibrary("lib_cons_comp.so")
        csolverlb.createCCSolver.restype = c_void_p
        csolverlb.createSet.argtypes = [c_void_p, c_uint, POINTER(c_long), c_uint]
        csolverlb.createSet.restype = c_void_p
        csolverlb.getElementVar.argtypes = [c_void_p, c_void_p]
        csolverlb.getElementVar.restype = c_void_p
        csolverlb.createPredicateOperator.argtypes = [c_void_p, c_uint, POINTER(c_void_p), c_uint]
        csolverlb.createPredicateOperator.restype = c_void_p
        csolverlb.applyPredicate.argtypes = [c_void_p, c_void_p, POINTER(c_void_p), c_uint]
        csolverlb.applyPredicate.restype = c_void_p
        csolverlb.addConstraint.argtypes = [c_void_p, c_void_p]
        csolverlb.addConstraint.restype = None
        csolverlb.solve.argtypes = [c_void_p]
        csolverlb.solve.restype = c_int
        csolverlb.getElementValue.argtypes = [c_void_p, c_void_p]
        csolverlb.getElementValue.restype = c_long
        csolverlb.deleteCCSolver.argtypes = [c_void_p]
        csolverlb.deleteCCSolver.restype = None
	return csolverlb

