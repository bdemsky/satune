#ifndef ARRAY_H
#define ARRAY_H

#define ArrayDef(name, type)																						\
	struct Array ## name {																								\
		type * array;                                                       \
		uint size;																													\
	};                                                                    \
	typedef struct Array ## name Array ## name;														\
	inline Array ## name * allocArray ## name(uint size) {								\
		Array ## name * tmp = (Array ## name *)ourmalloc(sizeof(type));			\
		tmp->size = size;																										\
		tmp->array = (type *) ourcalloc(1, sizeof(type) * size);						\
		return tmp;                                                         \
	}                                                                     \
	inline Array ## name * allocArrayInit ## name(type * array, uint size)  { \
		Array ## name * tmp = allocArray ## name(size);											\
		memcpy(tmp->array, array, size * sizeof(type));											\
		return tmp;                                                         \
	}                                                                     \
	inline type getArray ## name(Array ## name * This, uint index) {			\
		return This->array[index];																					\
	}                                                                     \
	inline void setArray ## name(Array ## name * This, uint index, type item) {	\
		This->array[index]=item;																						\
	}                                                                     \
	inline uint getSizeArray ## name(Array ## name *This) {								\
		return This->size;																									\
	}                                                                     \
	inline void deleteArray ## name(Array ## name *This) {								\
		ourfree(This->array);																								\
		ourfree(This);																											\
	}                                                                     \
	inline type * exposeCArray ## name(Array ## name * This) {							\
		return This->array;																									\
	}																																			\
	inline void deleteInlineArray ## name(Array ## name *This) {					\
		ourfree(This->array);																								\
	}																																			\
	inline void allocInlineArray ## name(Array ## name * This, uint size) {			\
		This->size = size;																									\
		This->array = (type *) ourcalloc(1, sizeof(type) * size);						\
	}																																			\
	inline void allocInlineArrayInit ## name(Array ## name * This, type *array, uint size) { \
		allocInlineArray ##name(This, size);																\
		memcpy(This->array, array, size * sizeof(type));										\
	}

#endif
