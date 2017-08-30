#ifndef OPS_H
#define OPS_H
enum LogicOp {SATC_AND, SATC_OR, SATC_NOT, SATC_XOR, SATC_IMPLIES};
typedef enum LogicOp LogicOp;

enum ArithOp {SATC_ADD, SATC_SUB};
typedef enum ArithOp ArithOp;

enum CompOp {SATC_EQUALS, SATC_LT, SATC_GT, SATC_LTE, SATC_GTE};
typedef enum CompOp CompOp;

enum OrderType {SATC_PARTIAL, SATC_TOTAL};
typedef enum OrderType OrderType;

enum HappenedBefore {SATC_FIRST, SATC_SECOND, SATC_UNORDERED};
typedef enum HappenedBefore HappenedBefore;

/**
 *    FLAGFORCESOVERFLOW forces the operation to overflow if the boolean flag is true
 *  OVERFLOWSETSFLAG -- sets the flag if the operation overflows
 *  FLAGIFFOVERFLOW -- flag is set iff the operation overflows
 *  SATC_IGNORE -- doesn't constrain output if the result cannot be represented
 *  SATC_WRAPAROUND -- wraps around like stand integer arithmetic
 *  NOOVERFLOW -- client has ensured that overflow is impossible
 */
enum OverFlowBehavior {SATC_IGNORE, SATC_WRAPAROUND, FLAGFORCESOVERFLOW, OVERFLOWSETSFLAG, FLAGIFFOVERFLOW, NOOVERFLOW};
typedef enum OverFlowBehavior OverFlowBehavior;

enum UndefinedBehavior {SATC_IGNOREBEHAVIOR, FLAGFORCEUNDEFINED, UNDEFINEDSETSFLAG, FLAGIFFUNDEFINED};
typedef enum UndefinedBehavior UndefinedBehavior;

enum FunctionType {TABLEFUNC, OPERATORFUNC};
typedef enum FunctionType FunctionType;

enum PredicateType {TABLEPRED, OPERATORPRED};
typedef enum PredicateType PredicateType;

enum ASTNodeType {ORDERCONST, BOOLEANVAR, LOGICOP, PREDICATEOP, BOOLCONST, ELEMSET, ELEMFUNCRETURN, ELEMCONST};
typedef enum ASTNodeType ASTNodeType;

enum Polarity {P_UNDEFINED=0, P_TRUE=1, P_FALSE=2, P_BOTHTRUEFALSE=3};
typedef enum Polarity Polarity;

enum BooleanValue {BV_UNDEFINED=0, BV_MUSTBETRUE=1, BV_MUSTBEFALSE=2, BV_UNSAT=3};
typedef enum BooleanValue BooleanValue;

#endif
