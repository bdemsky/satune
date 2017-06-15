#ifndef VECTOR_H
#define VECTOR_H
#include <string.h>

#define VectorDef(name, type, defcap)																		\
	struct Vector ## name {																								\
		uint size;																													\
		uint capacity;																											\
		type * array;																												\
	};																																		\
	typedef struct Vector ## name Vector ## name;													\
	Vector ## name * allocVector ## name(uint capacity);									\
	Vector ## name * allocDefVector ## name();														\
	Vector ## name * allocVectorArray ## name(uint capacity, type * array); \
	void pushVector ## name(Vector ## name *vector, type item);						\
	type getVector ## name(Vector ## name *vector, uint index);						\
	void setVector ## name(Vector ## name *vector, uint index, type item); \
	uint getSizeVector ##name(Vector ##name *vector);											\
	void freeVector ##name(Vector ##name *vector);

#define VectorImpl(name, type, defcap)																	\
	Vector ## name * allocDefVector ## name() {														\
		return allocVector ## name(defcap);																	\
	}																																			\
	Vector ## name * allocVector ## name(uint capacity)	{									\
		Vector ## name * tmp = (Vector ## name *) ourmalloc(sizeof(type));	\
		tmp->size = 0;																											\
		tmp->capacity = capacity;																						\
		tmp->array = (type *) ourcalloc(1, sizeof(type) * capacity);					\
		return tmp;																													\
	}																																			\
	Vector ## name * allocVectorArray ## name(uint capacity, type * array)	{	\
		Vector ## name * tmp = allocVector ## name(capacity);								\
		memcpy(tmp->array, array, capacity * sizeof(type));									\
		return tmp;																													\
	}																																			\
	void pushVector ## name(Vector ## name *vector, type item) {					\
		if (vector->size >= vector->capacity) {															\
			uint newcap=vector->capacity * 2;																	\
			vector->array=(type *)ourrealloc(vector->array, newcap);					\
		}																																		\
		vector->array[vector->size++] = item;																\
	}																																			\
	type getVector ## name(Vector ## name * vector, uint index) {					\
		return vector->array[index];																				\
	}																																			\
	void setVector ## name(Vector ## name * vector, uint index, type item) { \
		vector->array[index]=item;																					\
	}																																			\
	uint getSizeVector ## name(Vector ## name *vector) {									\
		return vector->size;																								\
	}																																			\
	void freeVector ##name(Vector ##name *vector) {												\
		ourfree(vector->array);																							\
		ourfree(vector);																										\
	}
#endif
