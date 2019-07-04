package edu.tamu.aser.constraints;

import java.util.HashMap;

public class SatuneJavaAPI{
        
        private static Long order = null;
        private static Long satune = null;
        private static SatuneJavaAPI instance = null;
        public static Long maxRange = 0L;

        private SatuneJavaAPI(){
            System.loadLibrary("_cons_comp");    // loads lib_cons_comp.so
            satune = createCCSolver();
            Long set = createRangeSet(satune, 1, 0, maxRange);
            order = createOrder(satune, 1, set); // 0= Partial Order, 1 = total order
        }
        
        public static SatuneJavaAPI getInstance(){
            if(instance == null){
                instance = new SatuneJavaAPI();
            }
            return instance;
        }
        
        public Long lookupBooleanOrder(Long gid1, Long gid2) {
            return orderConstraint(satune,order, gid1, gid2);
        }
        
        public void addConstraint(Long constraint){
            addConstraint(satune, constraint);
        }
        
        public Long LogicalOperationTwo(int op, long arg1, long arg2){
            return applyLogicalOperationTwo(satune, op, arg1, arg2);
        }
        
        public Long getBooleanTrue(){
            return getBooleanTrue(satune);
        }
        
        public void print(){
            printConstraints(satune);
        }
	
        public void turnoffOptimizations(){
            turnoffOptimizations(satune);
        }
	
        public int solve(){
            return solve(satune);
        }
        private native long createCCSolver();
	private native void deleteCCSolver(long solver);
	private native long createSet(long solver, int type, long elements, int num);
	private native long createRangeSet(long solver,int type, long lowrange, long highrange);
	private native long createRangeVar(long solver,int type, long lowrange, long highrange);
	private native long createMutableSet(long solver, int type);
	private native void addItem(long solver,long set, long element);
	private native void finalizeMutableSet(long solver,long set);
	private native long getElementVar(long solver,long set);
	private native long getElementConst(long solver, int type, long value);
	private native long getElementRange (long solver,long element);
	private native long getBooleanVar(long solver, int type);
        private native long getBooleanTrue(long solver);
        private native long getBooleanFalse(long solver);
	private native long createFunctionOperator(long solver, int op, long range, int overflowbehavior);
	private native long createPredicateOperator(long solver, int op);
	private native long createPredicateTable(long solver,long table, int behavior);
	private native long createTable(long solver, long range);
	private native long createTableForPredicate(long solver);
	private native void addTableEntry(long solver,long table, long inputs, int inputSize, long result);
	private native long completeTable(long solver,long table, int behavior);
	private native long applyFunction(long solver,long function, long array, int numArrays, long overflowstatus);
	private native long applyPredicateTable(long solver,long predicate, long inputs, int numInputs, long undefinedStatus);
	private native long applyPredicate(long solver,long predicate, long inputs, int numInputs);
	private native long applyLogicalOperation(long solver, int op, long array, int asize);
	private native long applyLogicalOperationTwo(long solver, int op, long arg1, long arg2);
	private native long applyLogicalOperationOne(long solver, int op, long arg);
	private native void addConstraint(long solver,long constraint);
	private native long createOrder(long solver, int type, long set);
	private native long orderConstraint(long solver,long order, long first, long second);
	private native int solve(long solver);
	private native long getElementValue(long solver,long element);
	private native int getBooleanValue(long solver,long bool);
	private native int getOrderConstraintValue(long solver,long order, long first, long second);
	private native void printConstraints(long solver);
	private native void turnoffOptimizations(long solver);
	private native void serialize(long solver);
	private native void mustHaveValue(long solver, long element);
	private native void setInterpreter(long solver, int type);
	private native long clone(long solver);

	public static void main(String[] args)
	{
		SatuneJavaAPI satuneapi = SatuneJavaAPI.getInstance();
		long solver = SatuneJavaAPI.getInstance().createCCSolver();
		long constr = satuneapi.getBooleanVar(solver, 1);
		satuneapi.addConstraint(solver, constr);
		int value = satuneapi.solve(solver);
		if (value == 1) {
			System.out.println("B1 = " + satuneapi.getBooleanValue(solver, constr));
		} else {
			System.out.println("UNSAT");
		}
		satuneapi.deleteCCSolver(solver);
	}
}


