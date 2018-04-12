from ctypes import *

class LogicOps:
	SATC_AND=0
	SATC_OR=1
	SATC_NOT=2
	SATC_XOR=3
	SATC_IFF=4
	SATC_IMPLIES=5

class CompOp:
	SATC_EQUALS = 0
	SATC_LT=1
	SATC_GT=2
	SATC_LTE=3
	SATC_GTE=4

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
	csolverlb.createRangeSet.argtypes = [c_void_p, c_uint, c_long, c_long]
	csolverlb.createRangeSet.restype = c_void_p
	csolverlb.createRangeVar.argtypes = [c_void_p, c_uint, c_long, c_long]
	csolverlb.createRangeVar.restype = c_void_p
	csolverlb.createMutableSet.argtypes = [c_void_p, c_uint]
	csolverlb.createMutableSet.restype = c_void_p
	csolverlb.addItem.argtypes = [c_void_p, c_void_p, c_long]
	csolverlb.addItem.restype = None
	csolverlb.finalizeMutableSet.argtypes = [c_void_p, c_void_p]
	csolverlb.finalizeMutableSet.restype = None
	csolverlb.getElementVar.argtypes = [c_void_p, c_void_p]
	csolverlb.getElementVar.restype = c_void_p
	csolverlb.getElementConst.argtypes = [c_void_p, c_uint, c_long]
	csolverlb.getElementConst.restype = c_void_p
	csolverlb.getElementRange.argtypes = [c_void_p, c_void_p]
	csolverlb.getElementRange.restype = c_void_p
	csolverlb.getBooleanVar.argtypes = [c_void_p, c_uint]
	csolverlb.getBooleanVar.restype = c_void_p
	csolverlb.createFunctionOperator.argtypes = [c_void_p, c_uint, POINTER(c_void_p), c_uint, c_void_p, c_uint]
	csolverlb.createFunctionOperator.restype = c_void_p
	csolverlb.createPredicateOperator.argtypes = [c_void_p, c_uint, POINTER(c_void_p), c_uint]
	csolverlb.createPredicateOperator.restype = c_void_p
	csolverlb.createPredicateTable.argtypes = [c_void_p, c_void_p, c_uint]
	csolverlb.createPredicateTable.restype = c_void_p
	csolverlb.createTable.argtypes = [c_void_p, POINTER(c_void_p), c_uint, c_void_p]
	csolverlb.createTable.restype = c_void_p
	csolverlb.createTableForPredicate.argtypes = [c_void_p, POINTER(c_void_p), c_uint]
	csolverlb.createTableForPredicate.restype = c_void_p
	csolverlb.addTableEntry.argtypes = [c_void_p, c_void_p, c_void_p, c_uint, c_long]
	csolverlb.addTableEntry.restype = None
	csolverlb.completeTable.argtypes = [c_void_p, c_void_p, c_uint]
	csolverlb.completeTable.restype = c_void_p
	csolverlb.applyFunction.argtypes = [c_void_p, c_void_p, POINTER(c_void_p), c_uint, c_void_p]
	csolverlb.applyFunction.restype = c_void_p
	csolverlb.applyPredicateTable.argtypes = [c_void_p, c_void_p, POINTER(c_void_p), c_uint, c_void_p]
	csolverlb.applyPredicateTable.restype = c_void_p
	csolverlb.applyPredicate.argtypes = [c_void_p, c_void_p, POINTER(c_void_p), c_uint]
	csolverlb.applyPredicate.restype = c_void_p
	csolverlb.applyLogicalOperation.argtypes = [c_void_p, c_uint, c_void_p, c_uint]
	csolverlb.applyLogicalOperation.restype = c_void_p
	csolverlb.applyLogicalOperationTwo.argtypes = [c_void_p, c_uint, c_void_p, c_void_p]
	csolverlb.applyLogicalOperationTwo.restype = c_void_p
	csolverlb.applyLogicalOperationOne.argtypes = [c_void_p, c_uint, c_void_p]
	csolverlb.applyLogicalOperationOne.restype = c_void_p
	csolverlb.addConstraint.argtypes = [c_void_p, c_void_p]
	csolverlb.addConstraint.restype = None
	csolverlb.createOrder.argtypes = [c_void_p, c_uint, c_void_p]
	csolverlb.createOrder.restype = c_void_p
	csolverlb.orderConstraint.argtypes = [c_void_p, c_void_p, c_long, c_long]
	csolverlb.orderConstraint.restype = c_void_p
	csolverlb.solve.argtypes = [c_void_p]
	csolverlb.solve.restype = c_int
	csolverlb.getElementValue.argtypes = [c_void_p, c_void_p]
	csolverlb.getElementValue.restype = c_long
	csolverlb.getBooleanValue.argtypes = [c_void_p, c_void_p]
	csolverlb.getBooleanValue.restype = c_int
	csolverlb.getOrderConstraintValue.argtypes = [c_void_p, c_void_p, c_long, c_long]
	csolverlb.getOrderConstraintValue.restype = c_int
	csolverlb.printConstraints.argtypes = [c_void_p]
	csolverlb.printConstraints.restype = None
	return csolverlb

