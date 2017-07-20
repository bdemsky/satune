/*      Copyright (c) 2015 Regents of the University of California
 *
 *      Author: Brian Demsky <bdemsky@uci.edu>
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      version 2 as published by the Free Software Foundation.
 */

#ifndef HASH_SET_H
#define HASH_SET_H
#include "hashtable.h"

#define HashSetDef(Name, _Key)																					\
	struct LinkNode ## Name {                                             \
		_Key key;                                                           \
		struct LinkNode ## Name *prev;                                      \
		struct LinkNode ## Name *next;                                      \
	};                                                                    \
	typedef struct LinkNode ## Name LinkNode ## Name;                     \
	struct HashSet ## Name;                                               \
	typedef struct HashSet ## Name HashSet ## Name;                       \
	struct HSIterator ## Name {                                           \
		LinkNode ## Name *curr;                                             \
		LinkNode ## Name *last;                                             \
		HashSet ## Name * set;                                              \
	};                                                                    \
	typedef struct HSIterator ## Name HSIterator ## Name;                 \
	HashTableDef(Name ## Set, _Key, LinkNode ## Name *);									\
	HSIterator ## Name * allocHSIterator ## Name(LinkNode ## Name *_curr, HashSet ## Name * _set); \
	void deleteIter ## Name(HSIterator ## Name *hsit);                      \
	bool hasNext ## Name(HSIterator ## Name *hsit);                       \
	_Key next ## Name(HSIterator ## Name *hsit);                          \
	_Key currKey ## Name(HSIterator ## Name *hsit);                       \
	void removeIter ## Name(HSIterator ## Name *hsit);                        \
	struct HashSet ## Name {                                              \
		HashTable ## Name ## Set * table;                                   \
		LinkNode ## Name *list;                                             \
		LinkNode ## Name *tail;                                             \
	};                                                                    \
	typedef struct HashSet ## Name HashSet ## Name;                       \
                                                                        \
	HashSet ## Name * allocHashSet ## Name (unsigned int initialcapacity, double factor); \
	void deleteHashSet ## Name(struct HashSet ## Name * set);               \
	HashSet ## Name * copyHashSet ## Name(HashSet ## Name * set);                \
	void resetHashSet ## Name(HashSet ## Name * set);                         \
	bool addHashSet ## Name(HashSet ## Name * set,_Key key);                     \
	_Key getHashSet ## Name(HashSet ## Name * set,_Key key);                  \
	_Key getHashSetFirstKey ## Name(HashSet ## Name * set);                      \
	bool containsHashSet ## Name(HashSet ## Name * set,_Key key);             \
	bool removeHashSet ## Name(HashSet ## Name * set,_Key key);               \
	unsigned int getSizeHashSet ## Name(HashSet ## Name * set);               \
	bool isEmptyHashSet ## Name(HashSet ## Name * set);                          \
	HSIterator ## Name * iterator ## Name(HashSet ## Name * set);


#define HashSetImpl(Name, _Key, hash_function, equals)                  \
	HashTableImpl(Name ## Set, _Key, LinkNode ## Name *, hash_function, equals, ourfree); \
	HSIterator ## Name * allocHSIterator ## Name(LinkNode ## Name *_curr, HashSet ## Name * _set) { \
		HSIterator ## Name * hsit = (HSIterator ## Name *)ourmalloc(sizeof(HSIterator ## Name)); \
		hsit->curr=_curr;                                                   \
		hsit->set=_set;                                                     \
		return hsit;                                                        \
	}                                                                     \
                                                                        \
	void deleteIter ## Name(HSIterator ## Name *hsit) {                   \
		ourfree(hsit);                                                      \
	}                                                                     \
                                                                        \
	bool hasNext ## Name(HSIterator ## Name *hsit) {                      \
		return hsit->curr!=NULL;                                            \
	}                                                                     \
                                                                        \
	_Key next ## Name(HSIterator ## Name *hsit) {                         \
		_Key k=hsit->curr->key;                                             \
		hsit->last=hsit->curr;                                              \
		hsit->curr=hsit->curr->next;                                        \
		return k;                                                           \
	}                                                                     \
                                                                        \
	_Key currKey ## Name(HSIterator ## Name *hsit) {                      \
		return hsit->last->key;                                             \
	}                                                                     \
                                                                        \
	void removeIter ## Name(HSIterator ## Name *hsit) {                   \
		_Key k=hsit->last->key;                                             \
		removeHashSet ## Name(hsit->set, k);                                    \
	}                                                                     \
                                                                        \
	HashSet ## Name * allocHashSet ## Name (unsigned int initialcapacity, double factor) { \
		HashSet ## Name * set = (HashSet ## Name *)ourmalloc(sizeof(struct HashSet ## Name));  \
		set->table=allocHashTable ## Name ## Set(initialcapacity, factor);          \
		set->list=NULL;                                                     \
		set->tail=NULL;                                                     \
		return set;                                                         \
	}                                                                       \
                                                                        \
	void deleteHashSet ## Name(struct HashSet ## Name * set) {            \
		LinkNode ## Name *tmp=set->list;                                    \
		while(tmp!=NULL) {                                                  \
			LinkNode ## Name *tmpnext=tmp->next;                              \
			ourfree(tmp);                                                     \
			tmp=tmpnext;                                                      \
		}                                                                   \
		deleteHashTable ## Name ## Set(set->table);                         \
		ourfree(set);                                                       \
	}                                                                     \
                                                                        \
	HashSet ## Name * copyHashSet ## Name(HashSet ## Name * set) {               \
		HashSet ## Name *copy=allocHashSet ## Name(getCapacity ## Name ## Set(set->table), getLoadFactor ## Name ## Set(set->table)); \
		HSIterator ## Name * it=iterator ## Name(set);                      \
		while(hasNext ## Name(it))                                          \
			addHashSet ## Name(copy, next ## Name(it));                              \
		deleteIter ## Name(it);                                             \
		return copy;                                                        \
	}                                                                     \
                                                                        \
	void resetHashSet ## Name(HashSet ## Name * set) {                        \
		LinkNode ## Name *tmp=set->list;                                    \
		while(tmp!=NULL) {                                                  \
			LinkNode ## Name *tmpnext=tmp->next;                              \
			ourfree(tmp);                                                     \
			tmp=tmpnext;                                                      \
		}                                                                   \
		set->list=set->tail=NULL;                                           \
		reset ## Name ## Set(set->table);                                   \
	}                                                                     \
                                                                        \
	bool addHashSet ## Name(HashSet ## Name * set,_Key key) {                    \
		LinkNode ## Name * val=get ## Name ## Set(set->table, key);         \
		if (val==NULL) {                                                    \
			LinkNode ## Name * newnode=(LinkNode ## Name *)ourmalloc(sizeof(struct LinkNode ## Name)); \
			newnode->prev=set->tail;                                          \
			newnode->next=NULL;                                               \
			newnode->key=key;                                                 \
			if (set->tail!=NULL)                                              \
				set->tail->next=newnode;                                        \
			else                                                              \
				set->list=newnode;                                              \
			set->tail=newnode;                                                \
			put ## Name ## Set(set->table, key, newnode);                     \
			return true;                                                      \
		} else                                                              \
			return false;                                                     \
	}                                                                     \
                                                                        \
	_Key getHashSet ## Name(HashSet ## Name * set,_Key key) {                 \
		LinkNode ## Name * val=get ## Name ## Set(set->table, key);         \
		if (val!=NULL)                                                      \
			return val->key;                                                  \
		else                                                                \
			return NULL;                                                      \
	}                                                                     \
                                                                        \
	_Key getHashSetFirstKey ## Name(HashSet ## Name * set) {                     \
		return set->list->key;                                              \
	}                                                                     \
                                                                        \
	bool containsHashSet ## Name(HashSet ## Name * set,_Key key) {            \
		return get ## Name ## Set(set->table, key)!=NULL;                   \
	}                                                                     \
                                                                        \
	bool removeHashSet ## Name(HashSet ## Name * set,_Key key) {              \
		LinkNode ## Name * oldlinknode;                                     \
		oldlinknode=get ## Name ## Set(set->table, key);                    \
		if (oldlinknode==NULL) {                                            \
			return false;                                                     \
		}                                                                   \
		remove ## Name ## Set(set->table, key);                             \
                                                                        \
		if (oldlinknode->prev==NULL)                                        \
			set->list=oldlinknode->next;                                      \
		else                                                                \
			oldlinknode->prev->next=oldlinknode->next;                        \
		if (oldlinknode->next!=NULL)                                        \
			oldlinknode->next->prev=oldlinknode->prev;                        \
		else                                                                \
			set->tail=oldlinknode->prev;                                      \
		ourfree(oldlinknode);                                               \
		return true;                                                        \
	}                                                                     \
                                                                        \
	unsigned int getSizeHashSet ## Name(HashSet ## Name * set) {              \
		return getSizeTable ## Name ## Set(set->table);                     \
	}                                                                     \
                                                                        \
	bool isEmptyHashSet ## Name(HashSet ## Name * set) {                         \
		return getSizeHashSet ## Name(set)==0;                                  \
	}                                                                     \
                                                                        \
	HSIterator ## Name * iterator ## Name(HashSet ## Name * set) {        \
		return allocHSIterator ## Name(set->list, set);                     \
	}
#endif
