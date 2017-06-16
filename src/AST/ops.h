#ifndef OPS_H
#define OPS_H
enum LogicOp {AND, OR, NOT, XOR, IMPLIES};
enum ArithOp {ADD, SUB};
enum CompOp {EQUALS};
enum OrderType {PARTIAL, TOTAL};

/**
 *    FLAGFORCESOVERFLOW forces the operation to overflow if the boolean flag is true
 *  OVERFLOWSETSFLAG -- sets the flag if the operation overflows
 *  FLAGIFFOVERFLOW -- flag is set iff the operation overflows
 *  IGNORE -- doesn't constrain output if the result cannot be represented
 *  WRAPAROUND -- wraps around like stand integer arithmetic
 */
enum OverFlowBehavior {IGNORE, WRAPAROUND, FLAGFORCESOVERFLOW, OVERFLOWSETSFLAG, FLAGIFFOVERFLOW};

enum BooleanType {_ORDER, _BOOLEAN};

#endif
