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

#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mymemory.h"
#include "common.h"

#define HT_DEFAULT_FACTOR 0.75
#define HT_INITIAL_CAPACITY 16

/**
 * @brief A simple, custom hash table
 *
 * By default it is model_malloc, but you can pass in your own allocation
 * functions. Note that This table does not support the value 0 (NULL) used as
 * a key and is designed primarily with pointer-based keys in mind. Other
 * primitive key types are supported only for non-zero values.
 *
 * @tparam _Key    Type name for the key
 * @tparam _Val    Type name for the values to be stored
 */
#define HashTableDef(Name, _Key, _Val)\
	struct hashlistnode ## Name {                                         \
		_Key key;                                                             \
		_Val val;                                                             \
	};                                                                    \
                                                                        \
	struct HashTable ## Name {                                            \
		struct hashlistnode ## Name *table;                                   \
		struct hashlistnode ## Name  *zero;                                   \
		unsigned int capacity;                                                \
		unsigned int size;                                                    \
		unsigned int capacitymask;                                            \
		unsigned int threshold;                                               \
		double loadfactor;                                                    \
	};                                                                    \
                                                                        \
	typedef struct HashTable ## Name HashTable ## Name;                   \
	HashTable ## Name * allocHashTable ## Name(unsigned int initialcapacity, double factor); \
	void deleteHashTable ## Name(HashTable ## Name * tab);                  \
	void reset ## Name(HashTable ## Name * tab);                          \
	void resetandfree ## Name(HashTable ## Name * tab);                   \
	void put ## Name(HashTable ## Name * tab, _Key key, _Val val);        \
	_Val get ## Name(const HashTable ## Name * tab, _Key key);            \
	_Val remove ## Name(HashTable ## Name * tab, _Key key);               \
	unsigned int getSizeTable ## Name(const HashTable ## Name * tab);     \
	bool contains ## Name(const HashTable ## Name * tab, _Key key);       \
	void resize ## Name(HashTable ## Name * tab, unsigned int newsize);   \
	double getLoadFactor ## Name(HashTable ## Name * tab);                \
	unsigned int getCapacity ## Name(HashTable ## Name * tab);						\
	void resetAndDeleteHashTable ## Name(HashTable ## Name * tab);

#define HashTableImpl(Name, _Key, _Val, hash_function, equals, freefunction) \
	HashTable ## Name * allocHashTable ## Name(unsigned int initialcapacity, double factor) { \
		HashTable ## Name * tab = (HashTable ## Name *)ourmalloc(sizeof(HashTable ## Name)); \
		tab->table = (struct hashlistnode ## Name *)ourcalloc(initialcapacity, sizeof(struct hashlistnode ## Name)); \
		tab->zero = NULL;                                                   \
		tab->loadfactor = factor;                                           \
		tab->capacity = initialcapacity;                                    \
		tab->capacitymask = initialcapacity - 1;                            \
                                                                        \
		tab->threshold = (unsigned int)(initialcapacity * factor);          \
		tab->size = 0;                                                      \
		return tab;                                                         \
	}                                                                     \
																																				\
	void deleteHashTable ## Name(HashTable ## Name * tab) {								\
		ourfree(tab->table);                                                \
		if (tab->zero)                                                      \
			ourfree(tab->zero);                                               \
		ourfree(tab);                                                       \
	}                                                                     \
																																				\
	void resetAndDeleteHashTable ## Name(HashTable ## Name * tab) {				\
		for(uint i=0;i<tab->capacity;i++) {																	\
			struct hashlistnode ## Name * bin=&tab->table[i];									\
			if (bin->key!=NULL) {																							\
				bin->key=NULL;																									\
				if (bin->val!=NULL) {																						\
					freefunction(bin->val);																				\
					bin->val=NULL;																								\
				}																																\
			}																																	\
		}																																		\
		if (tab->zero)	{																										\
			if (tab->zero->val != NULL)																				\
				freefunction(tab->zero->val);																		\
			ourfree(tab->zero);																								\
			tab->zero=NULL;																										\
		}																																		\
		tab->size=0;																												\
	}																																			\
																																				\
	void reset ## Name(HashTable ## Name * tab) {                         \
		memset(tab->table, 0, tab->capacity * sizeof(struct hashlistnode ## Name)); \
		if (tab->zero) {                                                    \
			ourfree(tab->zero);                                               \
			tab->zero = NULL;                                                 \
		}                                                                   \
		tab->size = 0;                                                      \
	}                                                                     \
                                                                        \
	void resetandfree ## Name(HashTable ## Name * tab) {                  \
		for(unsigned int i=0;i<tab->capacity;i++) {                         \
			struct hashlistnode ## Name *bin = &tab->table[i];                \
			if (bin->key != NULL) {                                           \
				bin->key = NULL;                                                \
				if (bin->val != NULL) {                                         \
					ourfree(bin->val);                                            \
					bin->val = NULL;                                              \
				}                                                               \
			}                                                                 \
		}                                                                   \
		if (tab->zero) {                                                    \
			if (tab->zero->val != NULL)                                       \
				ourfree(tab->zero->val);                                        \
			ourfree(tab->zero);                                               \
			tab->zero = NULL;                                                 \
		}                                                                   \
		tab->size = 0;                                                      \
	}                                                                     \
                                                                        \
	void put ## Name(HashTable ## Name * tab, _Key key, _Val val) {       \
		if (!key) {                                                         \
			if (!tab->zero) {                                                 \
				tab->zero=(struct hashlistnode ## Name *)ourmalloc(sizeof(struct hashlistnode ## Name)); \
				tab->size++;                                                    \
			}                                                                 \
			tab->zero->key=key;                                               \
			tab->zero->val=val;                                               \
			return;                                                           \
		}                                                                   \
                                                                        \
		if (tab->size > tab->threshold)                                     \
			resize ## Name (tab, tab->capacity << 1);                         \
                                                                        \
		struct hashlistnode ## Name *search;                                \
                                                                        \
		unsigned int index = hash_function(key);                            \
		do {                                                                \
			index &= tab->capacitymask;                                       \
			search = &tab->table[index];                                      \
			if (!search->key) {                                               \
				break;                                                          \
			}                                                                 \
			if (equals(search->key, key)) {                                   \
				search->val = val;                                              \
				return;                                                         \
			}                                                                 \
			index++;                                                          \
		} while (true);                                                     \
                                                                        \
		search->key = key;                                                  \
		search->val = val;                                                  \
		tab->size++;                                                        \
	}                                                                     \
                                                                        \
	_Val get ## Name(const HashTable ## Name * tab, _Key key) {         \
		struct hashlistnode ## Name *search;                                \
                                                                        \
		if (!key) {                                                         \
			if (tab->zero)                                                    \
				return tab->zero->val;                                          \
			else                                                              \
				return (_Val) 0;                                                \
		}                                                                   \
                                                                        \
		unsigned int oindex = hash_function(key) & tab->capacitymask;       \
		unsigned int index=oindex;                                          \
		do {                                                                \
			search = &tab->table[index];                                      \
			if (!search->key) {                                               \
				if (!search->val)                                               \
					break;                                                        \
			} else                                                            \
			if (equals(search->key, key))                                   \
				return search->val;                                           \
			index++;                                                          \
			index &= tab->capacitymask;                                       \
			if (index==oindex)                                                \
				break;                                                          \
		} while (true);                                                     \
		return (_Val)0;                                                     \
	}                                                                     \
                                                                        \
	_Val remove ## Name(HashTable ## Name * tab, _Key key) {              \
		struct hashlistnode ## Name *search;                                \
                                                                        \
		if (!key) {                                                         \
			if (!tab->zero) {                                                 \
				return (_Val)0;                                                 \
			} else {                                                          \
				_Val v=tab->zero->val;                                          \
				ourfree(tab->zero);                                             \
				tab->zero=NULL;                                                 \
				tab->size--;                                                    \
				return v;                                                       \
			}                                                                 \
		}                                                                   \
                                                                        \
		unsigned int index = hash_function(key);                            \
		do {                                                                \
			index &= tab->capacitymask;                                       \
			search = &tab->table[index];                                      \
			if (!search->key) {                                               \
				if (!search->val)                                               \
					break;                                                        \
			} else                                                            \
			if (equals(search->key, key)) {                                 \
				_Val v=search->val;                                           \
				search->val=(_Val) 1;                                         \
				search->key=0;                                                \
				tab->size--;                                                  \
				return v;                                                     \
			}                                                               \
			index++;                                                          \
		} while (true);                                                     \
		return (_Val)0;                                                     \
	}                                                                     \
                                                                        \
	unsigned int getSizeTable ## Name(const HashTable ## Name * tab) {    \
		return tab->size;                                                   \
	}                                                                     \
                                                                        \
                                                                        \
	bool contains ## Name(const HashTable ## Name * tab, _Key key) {      \
		struct hashlistnode ## Name *search;                                \
		if (!key) {                                                         \
			return tab->zero!=NULL;                                           \
		}                                                                   \
		unsigned int index = hash_function(key);                            \
		do {                                                                \
			index &= tab->capacitymask;                                       \
			search = &tab->table[index];                                      \
			if (!search->key) {                                               \
				if (!search->val)                                               \
					break;                                                        \
			} else                                                            \
			if (equals(search->key, key))                                   \
				return true;                                                  \
			index++;                                                          \
		} while (true);                                                     \
		return false;                                                       \
	}                                                                     \
                                                                        \
	void resize ## Name(HashTable ## Name * tab, unsigned int newsize) {  \
		struct hashlistnode ## Name *oldtable = tab->table;                 \
		struct hashlistnode ## Name *newtable;                              \
		unsigned int oldcapacity = tab->capacity;                           \
                                                                        \
		if ((newtable = (struct hashlistnode ## Name *)ourcalloc(newsize, sizeof(struct hashlistnode ## Name))) == NULL) { \
			model_print("calloc error %s %d\n", __FILE__, __LINE__);          \
			exit(EXIT_FAILURE);                                               \
		}                                                                   \
                                                                        \
		tab->table = newtable;                                              \
		tab->capacity = newsize;                                            \
		tab->capacitymask = newsize - 1;                                    \
                                                                        \
		tab->threshold = (unsigned int)(newsize * tab->loadfactor);         \
                                                                        \
		struct hashlistnode ## Name *bin = &oldtable[0];                    \
		struct hashlistnode ## Name *lastbin = &oldtable[oldcapacity];      \
		for (;bin < lastbin;bin++) {                                        \
			_Key key = bin->key;                                              \
                                                                        \
			struct hashlistnode ## Name *search;                              \
			if (!key)                                                         \
				continue;                                                       \
                                                                        \
			unsigned int index = hash_function(key);                          \
			do {                                                              \
				index &= tab->capacitymask;                                     \
				search = &tab->table[index];                                    \
				index++;                                                        \
			} while (search->key);                                            \
                                                                        \
			search->key = key;                                                \
			search->val = bin->val;                                           \
		}                                                                   \
                                                                        \
		ourfree(oldtable);                                                  \
	}                                                                     \
	double getLoadFactor ## Name(HashTable ## Name * tab) {return tab->loadfactor;} \
	unsigned int getCapacity ## Name(HashTable ## Name * tab) {return tab->capacity;}




#endif/* __HASHTABLE_H__ */
