#ifndef PREPROCESS_H
#define PREPROCESS_H
#include "classlist.h"
#include "transform.h"

class Preprocess : public Transform {
public:
	Preprocess(CSolver *_solver);
	~Preprocess();
	void doTransform();

	CMEMALLOC;
private:
	HashsetBoolean toremove;
	void processBooleanVar(BooleanVar *b);
	void resolveBooleanVars();
};

#endif
