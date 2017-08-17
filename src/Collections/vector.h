#ifndef VECTOR_H
#define VECTOR_H
#include <string.h>

#define VectorDef(name, type)                                           \
	struct Vector ## name {                                               \
		uint size;                                                          \
		uint capacity;                                                      \
		type *array;                                                       \
	};                                                                    \
	typedef struct Vector ## name Vector ## name;                         \
	Vector ## name * allocVector ## name(uint capacity);                  \
	Vector ## name * allocDefVector ## name();                            \
	Vector ## name * allocVectorArray ## name(uint capacity, type * array); \
	void pushVector ## name(Vector ## name * vector, type item);           \
	type lastVector ## name(Vector ## name * vector);                      \
	void popVector ## name(Vector ## name * vector);                       \
	type getVector ## name(Vector ## name * vector, uint index);           \
	void setVector ## name(Vector ## name * vector, uint index, type item); \
	uint getSizeVector ## name(Vector ## name * vector);                   \
	void setSizeVector ## name(Vector ## name * vector, uint size);        \
	void deleteVector ## name(Vector ## name * vector);                    \
	void clearVector ## name(Vector ## name * vector);                     \
	void deleteVectorArray ## name(Vector ## name * vector);               \
	type *exposeArray ## name(Vector ## name * vector);                  \
	void initVector ## name(Vector ## name * vector, uint capacity); \
	void initDefVector ## name(Vector ## name * vector);            \
	void initVectorArray ## name(Vector ## name * vector, uint capacity, type * array);

#define VectorImpl(name, type, defcap)                                  \
	Vector ## name * allocDefVector ## name() {                           \
		return allocVector ## name(defcap);                                 \
	}                                                                     \
	Vector ## name * allocVector ## name(uint capacity) {                 \
		Vector ## name * tmp = (Vector ## name *)ourmalloc(sizeof(Vector ## name));  \
		tmp->size = 0;                                                      \
		tmp->capacity = capacity;                                           \
		tmp->array = (type *) ourmalloc(sizeof(type) * capacity);           \
		return tmp;                                                         \
	}                                                                     \
	Vector ## name * allocVectorArray ## name(uint capacity, type * array)  { \
		Vector ## name * tmp = allocVector ## name(capacity);               \
		tmp->size = capacity;                                                 \
		memcpy(tmp->array, array, capacity * sizeof(type));                 \
		return tmp;                                                         \
	}                                                                     \
	void popVector ## name(Vector ## name * vector) {                      \
		vector->size--;                                                     \
	}                                                                     \
	type lastVector ## name(Vector ## name * vector) {                     \
		return vector->array[vector->size - 1];                               \
	}                                                                     \
	void setSizeVector ## name(Vector ## name * vector, uint size) {       \
		if (size <= vector->size) {                                         \
			vector->size = size;                                                \
			return;                                                           \
		} else if (size > vector->capacity) {                               \
			vector->array = (type *)ourrealloc(vector->array, size * sizeof(type)); \
			vector->capacity = size;                                            \
		}                                                                   \
		bzero(&vector->array[vector->size], (size - vector->size) * sizeof(type)); \
		vector->size = size;                                                  \
	}                                                                     \
	void pushVector ## name(Vector ## name * vector, type item) {          \
		if (vector->size >= vector->capacity) {                             \
			uint newcap = vector->capacity << 1;                                \
			vector->array = (type *)ourrealloc(vector->array, newcap * sizeof(type)); \
			vector->capacity = newcap;                                          \
		}                                                                   \
		vector->array[vector->size++] = item;                               \
	}                                                                     \
	type getVector ## name(Vector ## name * vector, uint index) {         \
		return vector->array[index];                                        \
	}                                                                     \
	void setVector ## name(Vector ## name * vector, uint index, type item) { \
		vector->array[index] = item;                                          \
	}                                                                     \
	uint getSizeVector ## name(Vector ## name * vector) {                  \
		return vector->size;                                                \
	}                                                                     \
	void deleteVector ## name(Vector ## name * vector) {                     \
		ourfree(vector->array);                                             \
		ourfree(vector);                                                    \
	}                                                                     \
	void clearVector ## name(Vector ## name * vector) {                     \
		vector->size = 0;                                                     \
	}                                                                     \
	type *exposeArray ## name(Vector ## name * vector) {                 \
		return vector->array;                                               \
	}                                                                     \
	void deleteVectorArray ## name(Vector ## name * vector) {              \
		ourfree(vector->array);                                             \
	}                                                                     \
	void initVector ## name(Vector ## name * vector, uint capacity) { \
		vector->size = 0;                                                      \
		vector->capacity = capacity;                                              \
		vector->array = (type *) ourmalloc(sizeof(type) * capacity);        \
	}                                                                     \
	void initDefVector ## name(Vector ## name * vector) {         \
		initVector ## name(vector, defcap);                         \
	}                                                                     \
	void initVectorArray ## name(Vector ## name * vector, uint capacity, type * array) {  \
		initVector ## name(vector, capacity);                          \
		vector->size = capacity;                                              \
		memcpy(vector->array, array, capacity * sizeof(type));  \
	}
#endif
