
public class SatuneJavaAPI{
	static {
		System.loadLibrary("_cons_comp");    // loads lib_cons_comp.so
	}
	public native long createCCSolver();
	public native void deleteCCSolver(long solver);
	public native long createSet(long solver, int type, long elements, int num);
	public native long createRangeSet(long solver,int type, long lowrange, long highrange);
	public native long createRangeVar(long solver,int type, long lowrange, long highrange);
	public native long createMutableSet(long solver, int type);
	public native void addItem(long solver,long set, long element);
	public native void finalizeMutableSet(long solver,long set);
	public native long getElementVar(long solver,long set);
	public native long getElementConst(long solver, int type, long value);
	public native long getElementRange (long solver,long element);
	public native long getBooleanVar(long solver, int type);
	public native long createFunctionOperator(long solver, int op, long range, int overflowbehavior);
	public native long createPredicateOperator(long solver, int op);
	public native long createPredicateTable(long solver,long table, int behavior);
	public native long createTable(long solver, long range);
	public native long createTableForPredicate(long solver);
	public native void addTableEntry(long solver,long table, long inputs, int inputSize, long result);
	public native long completeTable(long solver,long table, int behavior);
	public native long applyFunction(long solver,long function, long array, int numArrays, long overflowstatus);
	public native long applyPredicateTable(long solver,long predicate, long inputs, int numInputs, long undefinedStatus);
	public native long applyPredicate(long solver,long predicate, long inputs, int numInputs);
	public native long applyLogicalOperation(long solver, int op, long array, int asize);
	public native long applyLogicalOperationTwo(long solver, int op, long arg1, long arg2);
	public native long applyLogicalOperationOne(long solver, int op, long arg);
	public native void addConstraint(long solver,long constraint);
	public native long createOrder(long solver, int type, long set);
	public native long orderConstraint(long solver,long order, long first, long second);
	public native int solve(long solver);
	public native long getElementValue(long solver,long element);
	public native int getBooleanValue(long solver,long bool);
	public native int getOrderConstraintValue(long solver,long order, long first, long second);
	public native void printConstraints(long solver);
	public native void serialize(long solver);
	public native void mustHaveValue(long solver, long element);
	public native void setInterpreter(long solver, int type);
	public native long clone(long solver);

	public static void main(String[] args)
	{
		SatuneJavaAPI satuneapi = new SatuneJavaAPI();
		long solver = satuneapi.createCCSolver();
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


