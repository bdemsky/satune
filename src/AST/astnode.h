#ifndef ASTNODE_H
#define ASTNODE_H
#include "classlist.h"
#include "ops.h"

struct ASTNode {
	ASTNodeType type;
};

#define GETASTNODETYPE(o) (((ASTNode *) o)->type)
#endif
