/*      Copyright (c) 2015 Regents of the University of California
 *
 *      Author: Brian Demsky <bdemsky@uci.edu>
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      version 2 as published by the Free Software Foundation.
 */

/** @file hashtable.h
 *  @brief Hashtable.  Standard chained bucket variety.
 */

#ifndef HASHTABLE_H__
#define HASHTABLE_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mymemory.h"
#include "common.h"

/**
 * @brief Hashtable node
 *
 * @tparam _Key    Type name for the key
 * @tparam _Val    Type name for the values to be stored
 */
template<typename _Key, typename _Val>
struct Hashlistnode {
	_Key key;
	_Val val;
	uint hashcode;
};

template<typename _Key, int _Shift, typename _KeyInt>
inline unsigned int defaultHashFunction(_Key hash) {
	return (unsigned int)(((_KeyInt)hash) >> _Shift);
}

template<typename _Key>
inline bool defaultEquals(_Key key1, _Key key2) {
	return key1 == key2;
}

/**
 * @brief A simple, custom hash table
 *
 * By default it is snapshotting, but you can pass in your own allocation
 * functions. Note that this table does not support the value 0 (NULL) used as
 * a key and is designed primarily with pointer-based keys in mind. Other
 * primitive key types are supported only for non-zero values.
 *
 * @tparam _Key    Type name for the key
 * @tparam _Val    Type name for the values to be stored
 * @tparam _KeyInt Integer type that is at least as large as _Key. Used for key
 *                 manipulation and storage.
 * @tparam _Shift  Logical shift to apply to all keys. Default 0.
 */
template<typename _Key, typename _Val, typename _KeyInt, int _Shift = 0, unsigned int (*hash_function)(_Key) = defaultHashFunction<_Key, _Shift, _KeyInt>, bool (*equals)(_Key, _Key) = defaultEquals<_Key> >
class Hashtable {
public:
	/**
	 * @brief Hash table constructor
	 * @param initialcapacity Sets the initial capacity of the hash table.
	 * Default size 1024.
	 * @param factor Sets the percentage full before the hashtable is
	 * resized. Default ratio 0.5.
	 */
	Hashtable(unsigned int initialcapacity = 1024, double factor = 0.5) {
		// Allocate space for the hash table
		table = (struct Hashlistnode<_Key, _Val> *)ourcalloc(initialcapacity, sizeof(struct Hashlistnode<_Key, _Val>));
		zero = NULL;
		loadfactor = factor;
		capacity = initialcapacity;
		capacitymask = initialcapacity - 1;

		threshold = (unsigned int)(initialcapacity * loadfactor);
		size = 0;							// Initial number of elements in the hash
	}

	/** @brief Hash table destructor */
	~Hashtable() {
		ourfree(table);
		if (zero)
			ourfree(zero);
	}

	/** Override: new operator */
	void *operator new(size_t size) {
		return ourmalloc(size);
	}

	/** Override: delete operator */
	void operator delete(void *p, size_t size) {
		ourfree(p);
	}

	/** Override: new[] operator */
	void *operator new[](size_t size) {
		return ourmalloc(size);
	}

	/** Override: delete[] operator */
	void operator delete[](void *p, size_t size) {
		ourfree(p);
	}

	/** @brief Reset the table to its initial state. */
	void reset() {
		memset(table, 0, capacity * sizeof(struct Hashlistnode<_Key, _Val>));
		if (zero) {
			ourfree(zero);
			zero = NULL;
		}
		size = 0;
	}

	/** Doesn't work with zero value */
	_Val getRandomValue() {
		while (true) {
			unsigned int index = random() & capacitymask;
			struct Hashlistnode<_Key, _Val> *bin = &table[index];
			if (bin->key != NULL && bin->val != NULL) {
				return bin->val;
			}
		}
	}

	void resetAndDeleteKeys() {
		for (unsigned int i = 0; i < capacity; i++) {
			struct Hashlistnode<_Key, _Val> *bin = &table[i];
			if (bin->key != NULL) {
				delete bin->key;
				bin->key = NULL;
				if (bin->val != NULL) {
					bin->val = NULL;
				}
			}
		}
		if (zero) {
			ourfree(zero);
			zero = NULL;
		}
		size = 0;
	}

	void resetAndDeleteVals() {
		for (unsigned int i = 0; i < capacity; i++) {
			struct Hashlistnode<_Key, _Val> *bin = &table[i];
			if (bin->key != NULL) {
				bin->key = NULL;
				if (bin->val != NULL) {
					delete bin->val;
					bin->val = NULL;
				}
			}
		}
		if (zero) {
			if (zero->val != NULL)
				delete zero->val;
			ourfree(zero);
			zero = NULL;
		}
		size = 0;
	}

	void resetAndFreeVals() {
		for (unsigned int i = 0; i < capacity; i++) {
			struct Hashlistnode<_Key, _Val> *bin = &table[i];
			if (bin->key != NULL) {
				bin->key = NULL;
				if (bin->val != NULL) {
					ourfree(bin->val);
					bin->val = NULL;
				}
			}
		}
		if (zero) {
			if (zero->val != NULL)
				ourfree(zero->val);
			ourfree(zero);
			zero = NULL;
		}
		size = 0;
	}

	/**
	 * @brief Put a key/value pair into the table
	 * @param key The key for the new value; must not be 0 or NULL
	 * @param val The value to store in the table
	 */
	void put(_Key key, _Val val) {
		/* Hashtable cannot handle 0 as a key */
		if (!key) {
			if (!zero) {
				zero = (struct Hashlistnode<_Key, _Val> *)ourmalloc(sizeof(struct Hashlistnode<_Key, _Val>));
				size++;
			}
			zero->key = key;
			zero->val = val;
			return;
		}

		if (size > threshold)
			resize(capacity << 1);

		struct Hashlistnode<_Key, _Val> *search;

		unsigned int hashcode = hash_function(key);
		unsigned int index = hashcode;
		do {
			index &= capacitymask;
			search = &table[index];
			if (!search->key) {
				//key is null, probably done
				break;
			}
			if (search->hashcode == hashcode)
				if (equals(search->key, key)) {
					search->val = val;
					return;
				}
			index++;
		} while (true);

		search->key = key;
		search->val = val;
		search->hashcode = hashcode;
		size++;
	}

	/**
	 * @brief Lookup the corresponding value for the given key
	 * @param key The key for finding the value; must not be 0 or NULL
	 * @return The value in the table, if the key is found; otherwise 0
	 */
	_Val get(_Key key) const {
		struct Hashlistnode<_Key, _Val> *search;

		/* Hashtable cannot handle 0 as a key */
		if (!key) {
			if (zero)
				return zero->val;
			else
				return (_Val) 0;
		}

		unsigned int hashcode = hash_function(key);
		unsigned int oindex = hashcode & capacitymask;
		unsigned int index = oindex;
		do {
			search = &table[index];
			if (!search->key) {
				if (!search->val)
					break;
			} else
			if (hashcode == search->hashcode)
				if (equals(search->key, key))
					return search->val;
			index++;
			index &= capacitymask;
			if (index == oindex)
				break;
		} while (true);
		return (_Val)0;
	}

	/**
	 * @brief Remove the given key and return the corresponding value
	 * @param key The key for finding the value; must not be 0 or NULL
	 * @return The value in the table, if the key is found; otherwise 0
	 */
	_Val remove(_Key key) {
		struct Hashlistnode<_Key, _Val> *search;

		/* Hashtable cannot handle 0 as a key */
		if (!key) {
			if (!zero) {
				return (_Val)0;
			} else {
				_Val v = zero->val;
				ourfree(zero);
				zero = NULL;
				size--;
				return v;
			}
		}


		unsigned int hashcode = hash_function(key);
		unsigned int index = hashcode;
		do {
			index &= capacitymask;
			search = &table[index];
			if (!search->key) {
				if (!search->val)
					break;
			} else
			if (hashcode == search->hashcode)
				if (equals(search->key, key)) {
					_Val v = search->val;
					//empty out this bin
					search->val = (_Val) 1;
					search->key = 0;
					size--;
					return v;
				}
			index++;
		} while (true);
		return (_Val)0;
	}

	unsigned int getSize() const {
		return size;
	}


	/**
	 * @brief Check whether the table contains a value for the given key
	 * @param key The key for finding the value; must not be 0 or NULL
	 * @return True, if the key is found; false otherwise
	 */
	bool contains(_Key key) const {
		struct Hashlistnode<_Key, _Val> *search;

		/* Hashtable cannot handle 0 as a key */
		if (!key) {
			return zero != NULL;
		}

		unsigned int index = hash_function(key);
		unsigned int hashcode = index;
		do {
			index &= capacitymask;
			search = &table[index];
			if (!search->key) {
				if (!search->val)
					break;
			} else
			if (hashcode == search->hashcode)
				if (equals(search->key, key))
					return true;
			index++;
		} while (true);
		return false;
	}

	/**
	 * @brief Resize the table
	 * @param newsize The new size of the table
	 */
	void resize(unsigned int newsize) {
		struct Hashlistnode<_Key, _Val> *oldtable = table;
		struct Hashlistnode<_Key, _Val> *newtable;
		unsigned int oldcapacity = capacity;

		if ((newtable = (struct Hashlistnode<_Key, _Val> *)ourcalloc(newsize, sizeof(struct Hashlistnode<_Key, _Val>))) == NULL) {
			model_print("calloc error %s %d\n", __FILE__, __LINE__);
			exit(EXIT_FAILURE);
		}

		table = newtable;											// Update the global hashtable upon resize()
		capacity = newsize;
		capacitymask = newsize - 1;

		threshold = (unsigned int)(newsize * loadfactor);

		struct Hashlistnode<_Key, _Val> *bin = &oldtable[0];
		struct Hashlistnode<_Key, _Val> *lastbin = &oldtable[oldcapacity];
		for (; bin < lastbin; bin++) {
			_Key key = bin->key;

			struct Hashlistnode<_Key, _Val> *search;
			if (!key)
				continue;

			unsigned int hashcode = bin->hashcode;
			unsigned int index = hashcode;
			do {
				index &= capacitymask;
				search = &table[index];
				index++;
			} while (search->key);

			search->hashcode = hashcode;
			search->key = key;
			search->val = bin->val;
		}

		ourfree(oldtable);												// Free the memory of the old hash table
	}
	double getLoadFactor() {return loadfactor;}
	unsigned int getCapacity() {return capacity;}
	struct Hashlistnode<_Key, _Val> *table;
	struct Hashlistnode<_Key, _Val> *zero;
	unsigned int capacity;
	unsigned int size;
private:
	unsigned int capacitymask;
	unsigned int threshold;
	double loadfactor;
};

#endif/* __HASHTABLE_H__ */
