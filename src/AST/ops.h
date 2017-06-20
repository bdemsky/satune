#ifndef OPS_H
#define OPS_H
enum LogicOp {L_AND, L_OR, L_NOT, L_XOR, L_IMPLIES};
typedef enum LogicOp LogicOp;

enum ArithOp {ADD, SUB};
typedef enum ArithOp ArithOp;

enum CompOp {EQUALS};
typedef enum CompOp CompOp;

enum OrderType {PARTIAL, TOTAL};
typedef enum OrderType OrderType;

/**
 *    FLAGFORCESOVERFLOW forces the operation to overflow if the boolean flag is true
 *  OVERFLOWSETSFLAG -- sets the flag if the operation overflows
 *  FLAGIFFOVERFLOW -- flag is set iff the operation overflows
 *  IGNORE -- doesn't constrain output if the result cannot be represented
 *  WRAPAROUND -- wraps around like stand integer arithmetic
 *  NOOVERFLOW -- client has ensured that overflow is impossible
 */
enum OverFlowBehavior {IGNORE, WRAPAROUND, FLAGFORCESOVERFLOW, OVERFLOWSETSFLAG, FLAGIFFOVERFLOW, NOOVERFLOW};
typedef enum OverFlowBehavior OverFlowBehavior;

enum BooleanType {ORDERCONST, BOOLEANVAR, LOGICOP, COMPARE, PREDICATEOP, TABLEPREDICATEOP};
typedef enum BooleanType BooleanType;

enum FunctionType {TABLEFUNC, OPERATORFUNC};
typedef enum FunctionType FunctionType;

enum ElementType {ELEMSET, ELEMFUNCRETURN};
typedef enum ElementType ElementType;
#endif
