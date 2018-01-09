
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

#define SERIALBUFFERLENGTH 4096

Serializer::Serializer(const char *file) :
	buffer((char *) ourmalloc(SERIALBUFFERLENGTH)),
	bufferoffset(0),
	bufferlength(SERIALBUFFERLENGTH) {
	filedesc = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (filedesc < 0) {
		exit(-1);
	}
}

void Serializer::flushBuffer() {
	ssize_t datatowrite = bufferoffset;
	ssize_t index = 0;
	while (datatowrite) {
		ssize_t byteswritten = write(filedesc, &buffer[index], datatowrite);
		if (byteswritten == -1)
			exit(-1);
		datatowrite -= byteswritten;
		index += byteswritten;
	}
	bufferoffset = 0;
}

Serializer::~Serializer() {
	flushBuffer();
	if (-1 == close(filedesc)) {
		exit(-1);
	}
	ourfree(buffer);
}

void Serializer::mywrite(const void *__buf, size_t __n) {
	char *towrite = (char *) __buf;
	if (__n > SERIALBUFFERLENGTH * 2) {
		if (bufferoffset != 0)
			flushBuffer();
		while (__n > 0) {
			ssize_t result = write(filedesc, &towrite, __n);
			if (result != (ssize_t) __n)
				exit(-1);
			towrite += result;
			__n -= result;
		}
	} else {
		do  {
			uint spacefree = bufferlength - bufferoffset;
			uint datatowrite = spacefree > __n ? __n : spacefree;
			memcpy(&buffer[bufferoffset], towrite, datatowrite);
			bufferoffset += datatowrite;

			if (spacefree < __n) {
				flushBuffer();
				__n -= datatowrite;
				towrite += datatowrite;
			} else if (spacefree == __n) {
				flushBuffer();
				return;
			} else {
				return;
			}
		} while (true);
	}
}


void serializeBooleanEdge(Serializer *serializer, BooleanEdge be, bool isTopLevel) {
	if (be == BooleanEdge(NULL)) {
		return;
	}
	be.getBoolean()->serialize(serializer);
	ASTNodeType type = BOOLEANEDGE;
	serializer->mywrite(&type, sizeof(ASTNodeType));
	Boolean *boolean = be.getRaw();
	serializer->mywrite(&boolean, sizeof(Boolean *));
	serializer->mywrite(&isTopLevel, sizeof(bool));
}
