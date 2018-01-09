#ifndef ARRAY_H
#define ARRAY_H

template<typename type>
class Array {
public:
	Array(uint _size) :
		array((type *) ourcalloc(1, sizeof(type) * _size)),
		size(_size)
	{
	}

	Array(type *_array, uint _size) :
		array((type *) ourcalloc(1, sizeof(type) * _size)),
		size(_size) {
		memcpy(array, _array, _size * sizeof(type));
	}

	~Array() {
		ourfree(array);
	}

	void remove(uint index) {
		size--;
		for (; index < size; index++) {
			array[index] = array[index + 1];
		}
	}

	type get(uint index) const {
		return array[index];
	}

	void set(uint index, type item) {
		array[index] = item;
	}

	uint getSize() const {
		return size;
	}

	type *expose() {
		return array;
	}

private:
	type *array;
	uint size;
};


#endif
