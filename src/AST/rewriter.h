#ifndef REWRITER_H
#define REWRITER_H
#include "classlist.h"

void replaceBooleanWithTrue(Boolean * This);
void replaceBooleanWithFalse(Boolean * This);
void replaceBooleanWithBoolean(Boolean *oldb, Boolean *newb);
void handleXORTrue(BooleanLogic *This, Boolean *child);
void handleXORFalse(BooleanLogic *This, Boolean *child);
void handleIMPLIESTrue(BooleanLogic *This, Boolean *child);
void handleIMPLIESFalse(BooleanLogic *This, Boolean *child);
void handleANDTrue(BooleanLogic *This, Boolean *child);
void handleORFalse(BooleanLogic *This, Boolean *child);

#endif
