#ifndef ARRAY_H
#define ARRAY_H

#define ArrayDef(name, type)                                            \
	struct Array ## name {                                                \
		type *array;                                                       \
		uint size;                                                          \
	};                                                                    \
	typedef struct Array ## name Array ## name;                           \
	static inline Array ## name *allocArray ## name(uint size) {               \
		Array ## name * tmp = (Array ## name *)ourmalloc(sizeof(type));     \
		tmp->size = size;                                                   \
		tmp->array = (type *) ourcalloc(1, sizeof(type) * size);            \
		return tmp;                                                         \
	}                                                                     \
	static inline Array ## name *allocArrayInit ## name(type * array, uint size)  { \
		Array ## name * tmp = allocArray ## name(size);                     \
		memcpy(tmp->array, array, size * sizeof(type));                     \
		return tmp;                                                         \
	}                                                                     \
	static inline void removeElementArray ## name(Array ## name * This, uint index) { \
		This->size--;                                                       \
		for (; index < This->size; index++) {                                    \
			This->array[index] = This->array[index + 1];                          \
		}                                                                   \
	}                                                                     \
	static inline type getArray ## name(Array ## name * This, uint index) { \
		return This->array[index];                                          \
	}                                                                     \
	static inline void setArray ## name(Array ## name * This, uint index, type item) {  \
		This->array[index] = item;                                            \
	}                                                                     \
	static inline uint getSizeArray ## name(Array ## name * This) {                \
		return This->size;                                                  \
	}                                                                     \
	static inline void deleteArray ## name(Array ## name * This) {               \
		ourfree(This->array);                                               \
		ourfree(This);                                                      \
	}                                                                     \
	static inline type *exposeCArray ## name(Array ## name * This) {             \
		return This->array;                                                 \
	}                                                                     \
	static inline void deleteInlineArray ## name(Array ## name * This) {         \
		ourfree(This->array);                                               \
	}                                                                     \
	static inline void initArray ## name(Array ## name * This, uint size) {     \
		This->size = size;                                                  \
		This->array = (type *) ourcalloc(1, sizeof(type) * size);           \
	}                                                                     \
	static inline void initArrayInit ## name(Array ## name * This, type * array, uint size) { \
		initArray ## name(This, size);                                       \
		memcpy(This->array, array, size * sizeof(type));                    \
	}

#endif
