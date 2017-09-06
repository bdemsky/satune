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
				HashsetBoolean processed;
				HashsetBoolean toremove;
				void transformBoolean(Boolean *b);
				void processBooleanVar(BooleanVar * b);
				void processLogicOp(BooleanLogic * b);
				void resolveBooleanVars();
};

#endif
