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
};

#endif
