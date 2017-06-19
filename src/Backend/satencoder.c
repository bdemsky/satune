#include "satencoder.h"

SATEncoder * allocSATEncoder() {
	SATEncoder *This=ourmalloc(sizeof (SATEncoder));
	return This;
}

void deleteSATEncoder(SATEncoder *This) {
	ourfree(This);
}
