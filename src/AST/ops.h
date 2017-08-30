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
 *    SATC_FLAGFORCESOVERFLOW forces the operation to overflow if the boolean flag is true
 *  SATC_OVERFLOWSETSFLAG -- sets the flag if the operation overflows
 *  SATC_FLAGIFFOVERFLOW -- flag is set iff the operation overflows
 *  SATC_IGNORE -- doesn't constrain output if the result cannot be represented
 *  SATC_WRAPAROUND -- wraps around like stand integer arithmetic
 *  SATC_NOOVERFLOW -- client has ensured that overflow is impossible
 */
enum OverFlowBehavior {SATC_IGNORE, SATC_WRAPAROUND, SATC_FLAGFORCESOVERFLOW, SATC_OVERFLOWSETSFLAG, SATC_FLAGIFFOVERFLOW, SATC_NOOVERFLOW};
typedef enum OverFlowBehavior OverFlowBehavior;

enum UndefinedBehavior {SATC_IGNOREBEHAVIOR, SATC_FLAGFORCEUNDEFINED, SATC_UNDEFINEDSETSFLAG, SATC_FLAGIFFUNDEFINED};
typedef enum UndefinedBehavior UndefinedBehavior;

#endif
