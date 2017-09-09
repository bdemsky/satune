#ifndef ASTOPS_H
#define ASTOPS_H

enum FunctionType {TABLEFUNC, OPERATORFUNC};
typedef enum FunctionType FunctionType;

enum PredicateType {TABLEPRED, OPERATORPRED};
typedef enum PredicateType PredicateType;

enum ASTNodeType {ORDERCONST, BOOLEANVAR, LOGICOP, PREDICATEOP, BOOLCONST, ELEMSET, ELEMFUNCRETURN, ELEMCONST, BOOLEANEDGE, ORDERTYPE, SETTYPE};
typedef enum ASTNodeType ASTNodeType;

enum Polarity {P_UNDEFINED=0, P_TRUE=1, P_FALSE=2, P_BOTHTRUEFALSE=3};
typedef enum Polarity Polarity;

enum BooleanValue {BV_UNDEFINED=0, BV_MUSTBETRUE=1, BV_MUSTBEFALSE=2, BV_UNSAT=3};
typedef enum BooleanValue BooleanValue;

#endif
