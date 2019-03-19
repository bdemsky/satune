/*      Copyright (c) 2015 Regents of the University of California
 *
 *      Author: Brian Demsky <bdemsky@uci.edu>
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      version 2 as published by the Free Software Foundation.
 */

#ifndef HASHSET_H
#define HASHSET_H
#include "hashtable.h"

template<typename _Key>
struct Linknode {
	_Key key;
	Linknode<_Key> *prev;
	Linknode<_Key> *next;
};

template<typename _Key, typename _KeyInt, int _Shift, unsigned int (*hash_function)(_Key), bool (*equals)(_Key, _Key)>
class Hashset;

template<typename _Key, typename _KeyInt, int _Shift, unsigned int (*hash_function)(_Key) = defaultHashFunction<_Key, _Shift, _KeyInt>, bool (*equals)(_Key, _Key) = defaultEquals<_Key> >
class SetIterator {
public:
	SetIterator(Linknode<_Key> *_curr, Hashset <_Key, _KeyInt, _Shift, hash_function, equals> *_set) :
		curr(_curr),
		set(_set)
	{
	}

	SetIterator(SetIterator *s) : curr(s->curr),
		last(s->last),
		set(s->set) {
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

	bool hasNext() {
		return curr != NULL;
	}

	_Key next() {
		_Key k = curr->key;
		last = curr;
		curr = curr->next;
		return k;
	}

	_Key currKey() {
		return last->key;
	}

	void remove() {
		_Key k = last->key;
		set->remove(k);
	}

private:
	Linknode<_Key> *curr;
	Linknode<_Key> *last;
	Hashset <_Key, _KeyInt, _Shift, hash_function, equals> *set;
};

template<typename _Key, typename _KeyInt, int _Shift = 0, unsigned int (*hash_function)(_Key) = defaultHashFunction<_Key, _Shift, _KeyInt>, bool (*equals)(_Key, _Key) = defaultEquals<_Key> >
class Hashset {
public:
	Hashset(unsigned int initialcapacity = 16, double factor = 0.5) :
		table(new Hashtable<_Key, Linknode<_Key> *, _KeyInt, _Shift, hash_function, equals>(initialcapacity, factor)),
		list(NULL),
		tail(NULL)
	{
	}

	/** @brief Hashset destructor */
	~Hashset() {
		Linknode<_Key> *tmp = list;
		while (tmp != NULL) {
			Linknode<_Key> *tmpnext = tmp->next;
			ourfree(tmp);
			tmp = tmpnext;
		}
		delete table;
	}

	Hashset<_Key, _KeyInt, _Shift, hash_function, equals> *copy() {
		Hashset<_Key, _KeyInt, _Shift, hash_function, equals> *copy = new Hashset<_Key, _KeyInt, _Shift, hash_function, equals>(table->getCapacity(), table->getLoadFactor());
		SetIterator<_Key, _KeyInt, _Shift, hash_function, equals> *it = iterator();
		while (it->hasNext())
			copy->add(it->next());
		delete it;
		return copy;
	}

	void reset() {
		Linknode<_Key> *tmp = list;
		while (tmp != NULL) {
			Linknode<_Key> *tmpnext = tmp->next;
			ourfree(tmp);
			tmp = tmpnext;
		}
		list = tail = NULL;
		table->reset();
	}

	void resetAndDelete() {
		Linknode<_Key> *tmp = list;
		while (tmp != NULL) {
			Linknode<_Key> *tmpnext = tmp->next;
			ourfree(tmp);
			tmp = tmpnext;
		}
		list = tail = NULL;
		table->resetAndDeleteKeys();
	}

	/** @brief Adds a new key to the hashset.  Returns false if the key
	 *  is already present. */

	void addAll(Hashset<_Key, _KeyInt, _Shift, hash_function, equals> *table) {
		SetIterator<_Key, _KeyInt, _Shift, hash_function, equals> *it = iterator();
		while (it->hasNext())
			add(it->next());
		delete it;
	}

	/** @brief Adds a new key to the hashset.  Returns false if the key
	 *  is already present. */

	bool add(_Key key) {
		Linknode<_Key> *val = table->get(key);
		if (val == NULL) {
			Linknode<_Key> *newnode = (Linknode<_Key> *)ourmalloc(sizeof(struct Linknode<_Key>));
			newnode->prev = tail;
			newnode->next = NULL;
			newnode->key = key;
			if (tail != NULL)
				tail->next = newnode;
			else
				list = newnode;
			tail = newnode;
			table->put(key, newnode);
			return true;
		} else
			return false;
	}

	/** @brief Return random key from set. */

	_Key getRandomElement() {
		if (getSize() == 0)
			return NULL;
		else if (getSize() < 6) {
			uint count = random() % getSize();
			Linknode<_Key> *ptr = list;
			while (count > 0) {
				ptr = ptr->next;
				count--;
			}
			return ptr->key;
		} else
			return table->getRandomValue()->key;
	}

	/** @brief Gets the original key corresponding to this one from the
	 *  hashset.  Returns NULL if not present. */

	_Key get(_Key key) {
		Linknode<_Key> *val = table->get(key);
		if (val != NULL)
			return val->key;
		else
			return NULL;
	}

	_Key getFirstKey() {
		return list->key;
	}

	bool contains(_Key key) {
		return table->get(key) != NULL;
	}

	bool remove(_Key key) {
		Linknode<_Key> *oldlinknode;
		oldlinknode = table->get(key);
		if (oldlinknode == NULL) {
			return false;
		}
		table->remove(key);

		//remove link node from the list
		if (oldlinknode->prev == NULL)
			list = oldlinknode->next;
		else
			oldlinknode->prev->next = oldlinknode->next;
		if (oldlinknode->next != NULL)
			oldlinknode->next->prev = oldlinknode->prev;
		else
			tail = oldlinknode->prev;
		ourfree(oldlinknode);
		return true;
	}

	unsigned int getSize() const {
		return table->getSize();
	}

	bool isEmpty() const {
		return getSize() == 0;
	}

	SetIterator<_Key, _KeyInt, _Shift, hash_function, equals> *iterator() {
		return new SetIterator<_Key, _KeyInt, _Shift, hash_function, equals>(list, this);
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
private:
	Hashtable<_Key, Linknode<_Key> *, _KeyInt, _Shift, hash_function, equals> *table;
	Linknode<_Key> *list;
	Linknode<_Key> *tail;
};
#endif
