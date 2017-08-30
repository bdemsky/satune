#ifndef ASTNODE_H
#define ASTNODE_H
#include "classlist.h"
#include "ops.h"
#include "astops.h"

class ASTNode {
public:
	ASTNode(ASTNodeType _type) : type(_type) {}
	ASTNodeType type;
	MEMALLOC;
};

#endif
