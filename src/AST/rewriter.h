#ifndef REWRITER_H
#define REWRITER_H
#include "classlist.h"

void replaceBooleanWithTrue(CSolver * This, Boolean *bexpr);
void replaceBooleanWithFalse(CSolver * This, Boolean *bexpr);
void replaceBooleanWithBoolean(CSolver * This, Boolean *oldb, Boolean *newb);
void handleXORTrue(BooleanLogic *bexpr, Boolean *child);
void handleXORFalse(CSolver * This, BooleanLogic *bexpr, Boolean *child);
void handleIMPLIESTrue(CSolver * This, BooleanLogic *bexpr, Boolean *child);
void handleIMPLIESFalse(CSolver * This, BooleanLogic *bexpr, Boolean *child);
void handleANDTrue(CSolver * This, BooleanLogic *bexpr, Boolean *child);
void handleORFalse(CSolver * This, BooleanLogic *bexpr, Boolean *child);

#endif
