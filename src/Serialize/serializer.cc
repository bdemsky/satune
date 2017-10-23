
/*
 * File:   serializer.cc
 * Author: hamed
 *
 * Created on September 7, 2017, 3:38 PM
 */

#include "serializer.h"
#include "unistd.h"
#include "fcntl.h"
#include "boolean.h"

Serializer::Serializer(const char *file) {
	filedesc = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0666);

	if (filedesc < 0) {
		exit(-1);
	}
}

Serializer::~Serializer() {
	if (-1 == close(filedesc)) {
		exit(-1);
	}
}

void Serializer::mywrite(const void *__buf, size_t __n) {
	write (filedesc, __buf, __n);
}


void serializeBooleanEdge(Serializer *serializer, BooleanEdge be, bool isTopLevel) {
	if (be == BooleanEdge(NULL)){
                return;
        }
	be.getBoolean()->serialize(serializer);
	ASTNodeType type = BOOLEANEDGE;
	serializer->mywrite(&type, sizeof(ASTNodeType));
	Boolean *boolean = be.getRaw();
	serializer->mywrite(&boolean, sizeof(Boolean *));
        serializer->mywrite(&isTopLevel, sizeof(bool));
}