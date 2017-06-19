#ifndef SATENCODER_H
#define SATENCODER_H

#include "classlist.h"

struct SATEncoder {

};


SATEncoder * allocSATEncoder();
void deleteSATEncoder(SATEncoder *This);
void encodeAllSATEncoder(SATEncoder *This, CSolver *csolver);
void encodeConstraintSATEncoder(SATEncoder *This, Boolean *constraint);
#endif
