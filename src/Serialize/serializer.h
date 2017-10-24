
/*
 * File:   serializer.h
 * Author: hamed
 *
 * Created on September 7, 2017, 3:38 PM
 */

#ifndef SERIALIZER_H
#define SERIALIZER_H
#include "mymemory.h"
#include "classlist.h"
#include "structs.h"


class Serializer {
public:
	Serializer(const char *file);
	void mywrite(const void *__buf, size_t __n);
	inline bool isSerialized(void *obj);
	inline void addObject(void *obj) { map.put(obj, obj);}
	virtual ~Serializer();
	CMEMALLOC;
private:
	void flushBuffer();
	char * buffer;
	uint bufferoffset;
	uint bufferlength;
	int filedesc;
	CloneMap map;
};

inline bool Serializer::isSerialized(void *obj) {
	return map.contains(obj);
}




void serializeBooleanEdge(Serializer *serializer, BooleanEdge be, bool isTopLevel=false);

#endif/* SERIALIZER_H */

