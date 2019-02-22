#ifndef CPPVECTOR_H
#define CPPVECTOR_H
#include <string.h>

#define VECTOR_DEFCAP 8

template<typename type>
class Vector {
public:
	Vector(uint _capacity = VECTOR_DEFCAP) :
		size(0),
		capacity(_capacity),
		array((type *) ourmalloc(sizeof(type) * _capacity)) {
	}

	Vector(uint _capacity, type *_array)  :
		size(_capacity),
		capacity(_capacity),
		array((type *) ourmalloc(sizeof(type) * _capacity)) {
		memcpy(array, _array, capacity * sizeof(type));
	}

	Vector(Vector<type> *v) :
		size(v->size),
		capacity(v->capacity),
		array((type *) ourmalloc(sizeof(type) * v->capacity)) {
		memcpy(array, v->array, capacity * sizeof(type));
	}
	void pop() {
		size--;
	}

	type last() const {
		return array[size - 1];
	}

	void setSize(uint _size) {
		if (_size <= size) {
			size = _size;
			return;
		} else if (_size > capacity) {
			array = (type *)ourrealloc(array, _size * sizeof(type));
			capacity = _size;
		}
		bzero(&array[size], (_size - size) * sizeof(type));
		size = _size;
	}

	void push(type item) {
		if (size >= capacity) {
			uint newcap = capacity << 1;
			array = (type *)ourrealloc(array, newcap * sizeof(type));
			capacity = newcap;
		}
		array[size++] = item;
	}

	type get(uint index) const {
		return array[index];
	}

	void setExpand(uint index, type item) {
		if (index >= size)
			setSize(index + 1);
		set(index, item);
	}

	void set(uint index, type item) {
		array[index] = item;
	}

	void insertAt(uint index, type item) {
		setSize(size + 1);
		for (uint i = size - 1; i > index; i--) {
			set(i, get(i - 1));
		}
		array[index] = item;
	}

	void removeAt(uint index) {
		for (uint i = index; (i + 1) < size; i++) {
			set(i, get(i + 1));
		}
		setSize(size - 1);
	}

	inline uint getSize() const {
		return size;
	}

	~Vector() {
		ourfree(array);
	}

	void clear() {
		size = 0;
	}

	type *expose() {
		return array;
	}
	CMEMALLOC;
private:
	uint size;
	uint capacity;
	type *array;
};
#endif
