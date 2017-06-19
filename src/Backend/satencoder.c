#include "satencoder.h"
#include "structs.h"
#include "csolver.h"

SATEncoder * allocSATEncoder() {
	SATEncoder *This=ourmalloc(sizeof (SATEncoder));
	return This;
}

void deleteSATEncoder(SATEncoder *This) {
	ourfree(This);
}

void encodeAllSATEncoder(SATEncoder * This, CSolver *csolver) {
	VectorBoolean *constraints=csolver->constraints;
	uint size=getSizeVectorBoolean(constraints);
	for(uint i=0;i<size;i++) {
		Boolean *constraint=getVectorBoolean(constraints, i);
		encodeConstraintSATEncoder(This, constraint);
	}
}

void encodeConstraintSATEncoder(SATEncoder *This, Boolean *constraint) {

}
