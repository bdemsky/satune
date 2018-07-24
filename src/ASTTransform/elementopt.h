#ifndef ELEMENTOPT_H
#define ELEMENTOPT_H
#include "classlist.h"
#include "transform.h"

class ElementOpt : public Transform {
public:
	ElementOpt(CSolver *_solver);
	~ElementOpt();
	void doTransform();

	CMEMALLOC;
private:
	void processPredicate(BooleanPredicate *);
	void handlePredicateEquals(BooleanPredicate *pred, ElementSet *left, ElementConst *right);
	void handlePredicateInequality(BooleanPredicate *pred, ElementSet *left, ElementConst *right);
	void replaceVarWithConst(BooleanPredicate * pred, ElementSet *var, ElementConst * value);
	void constrainVarWithConst(BooleanPredicate *pred, ElementSet *var, ElementConst *value);

	Vector<BooleanPredicate *> workList;
	bool updateSets;
};

#endif
