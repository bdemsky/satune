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

#define HashSetDef(Name, _Key, _KeyInt, _Shift, hash_function, equals)	\
	struct LinkNode ## Name{																							\
	_Key key;																															\
	struct LinkNode ## Name *prev;																				\
	struct LinkNode ## Name *next;																				\
	};																																		\
	typedef struct LinkNode ## Name LinkNode ## Name;											\
	struct HashSet ## Name;																								\
	typedef struct HashSet ## Name HashSet ## Name;												\
	struct HSIterator ## Name {																						\
		LinkNode ## Name *curr;																							\
		LinkNode ## Name *last;																							\
		HashSet ## Name * set;																							\
	};																																		\
																																				\
	HSIterator ## Name * allocHSIterator ## Name(LinkNode ## Name *_curr, HashSet ## Name * _set); \
	void freeIter ## Name(HSIterator ## Name *hsit);											\
	bool hasNext ## Name(HSIterator ## Name *hsit);												\
	_Key next ## Name(HSIterator ## Name *hsit);													\
	_Key currKey ## Name(HSIterator ## Name *hsit);												\
	void remove ## Name(HSIterator ## Name *hsit);												\
	struct HashSet ## Name {																							\
		HashTable ## Name * table;																					\
		LinkNode ## Name *list;																							\
		LinkNode ## Name *tail;																							\
	};																																		\
	typedef struct HashSet ## Name HashSet ## Name;												\
																																				\
	HashSet ## Name * allocHashSet ## Name (unsigned int initialcapacity, double factor);	\
	void freeHashSet ## Name(struct HashSet ## Name * set);								\
	HashSet ## Name * copy ## Name(HashSet ## Name * set);								\
	void reset ## Name(HashSet ## Name * set);														\
	bool add ## Name(HashSet ## Name * set,_Key key);											\
	_Key get ## Name(HashSet ## Name * set,_Key key);											\
	_Key getFirstKey ## Name(HashSet ## Name * set);											\
	bool contains ## Name(HashSet ## Name * set,_Key key);								\
	bool remove ## Name(HashSet ## Name * set,_Key key);									\
	unsigned int getSize ## Name(HashSet ## Name * set);									\
	bool isEmpty ## Name(HashSet ## Name * set);													\
	HSIterator ## Name * iterator ## Name(HashSet ## Name * set);


#define HashSetDef(Name, _Key, _KeyInt, _Shift, hash_function, equals)	\
	HSIterator ## Name * allocHSIterator ## Name(LinkNode ## Name *_curr, HashSet ## Name * _set) { \
		HSIterator ## Name * hsit = (HSIterator ## Name *) ouralloc(sizeof(HSIterator ## Name)); \
		hsit->curr=_curr;																										\
		hsit->set=_set;																											\
	}																																			\
																																				\
	void freeIter ## Name(HSIterator ## Name *hsit) {											\
		ourfree(hsit);																											\
	}																																			\
																																				\
	bool hasNext ## Name(HSIterator ## Name *hsit) {											\
		return hsit->curr!=NULL;																						\
	}																																			\
																																				\
	_Key next ## Name(HSIterator ## Name *hsit) {													\
		_Key k=hsit->curr->key;																							\
		hsit->last=hsit->curr;																							\
		hsit->curr=hsit->curr->next;																				\
		return k;																														\
	}																																			\
																																				\
	_Key currKey ## Name(HSIterator ## Name *hsit) {											\
		return hsit->last->key;																							\
	}																																			\
																																				\
	void remove ## Name(HSIterator ## Name *hsit) {												\
		_Key k=hsit->last->key;																							\
		remove ## Name(hsit->set, k);																				\
	}																																			\
																																				\
	HashSet ## Name * allocHashSet ## Name (unsigned int initialcapacity, double factor) { \
		HashSet ## Name * set = (HashSet ## Name *) ouralloc(sizeof(struct HashSet ## Name));	\
		set->table=allocHashTable ## Name(initialcapcity, factor);					\
		set->list=NULL;																											\
		set->tail=NULL;																											\
	}																																			\
																																				\
	void freeHashSet ## Name(struct HashSet ## Name * set) {							\
		LinkNode ## Name *tmp=set->list;																		\
		while(tmp!=NULL) {																									\
			LinkNode ## Name *tmpnext=tmp->next;															\
			ourfree(tmp);																											\
			tmp=tmpnext;																											\
		}																																		\
		freeHashTable ## Name(set->table);																	\
		ourfree(set);																												\
	}																																			\
																																				\
	HashSet ## Name * copy ## Name(HashSet ## Name * set) {								\
		HashSet ## Name *copy=new HashSet ## Name(getCapacity ## Name(set->table), getLoadFactor ## Name(set->table)); \
		HSIterator ## Name * it=iterator ## Name(set);											\
		while(hasNext ## Name(it))																					\
			add ## Name(copy, next ## Name(it));															\
		freeIt ## Name(it);																									\
		return copy;																												\
	}																																			\
																																				\
	void reset ## Name(HashSet ## Name * set) {														\
		LinkNode ## Name *tmp=set->list;																		\
		while(tmp!=NULL) {																									\
			LinkNode ## Name *tmpnext=tmp->next;															\
			ourfree(tmp);																											\
			tmp=tmpnext;																											\
		}																																		\
		set->list=set->tail=NULL;																						\
		reset ## Name(set->table);																					\
	}																																			\
																																				\
	bool add ## Name(HashSet ## Name * set,_Key key) {										\
		LinkNode ## Name * val=get ## Name(set->table, key);								\
		if (val==NULL) {																										\
			LinkNode ## Name * newnode=(LinkNode ## Name *)ourmalloc(sizeof(struct LinkNode ## Name)); \
			newnode->prev=set->tail;																					\
			newnode->next=NULL;																								\
			newnode->key=key;																									\
			if (set->tail!=NULL)																							\
				set->tail->next=newnode;																				\
			else																															\
				set->list=newnode;																							\
			set->tail=newnode;																								\
			put ## Name(set->table, key, newnode);														\
			return true;																											\
		} else																															\
			return false;																											\
	}																																			\
																																				\
	_Key get ## Name(HashSet ## Name * set,_Key key) {										\
		LinkNode ## Name * val=get ## Name(set->table, key);								\
		if (val!=NULL)																											\
			return val->key;																									\
		else																																\
			return NULL;																											\
	}																																			\
																																				\
	_Key getFirstKey ## Name(HashSet ## Name * set) {											\
		return set->list->key;																							\
	}																																			\
																																				\
	bool contains ## Name(HashSet ## Name * set,_Key key) {								\
		return get ## Name(set->table, key)!=NULL;													\
	}																																			\
																																				\
	bool remove ## Name(HashSet ## Name * set,_Key key) {									\
		LinkNode ## Name * oldlinknode;																			\
		oldlinknode=get ## Name(set->table, key);														\
		if (oldlinknode==NULL) {																						\
			return false;																											\
		}																																		\
		remove ## Name(set->table, key);																		\
																																				\
		if (oldlinknode->prev==NULL)																				\
			set->list=oldlinknode->next;																			\
		else																																\
			oldlinknode->prev->next=oldlinknode->next;												\
		if (oldlinknode->next!=NULL)																				\
			oldlinknode->next->prev=oldlinknode->prev;												\
		else																																\
			set->tail=oldlinknode->prev;																			\
		ourfree(oldlinknode);																								\
		return true;																												\
	}																																			\
																																				\
	unsigned int getSize ## Name(HashSet ## Name * set) {									\
		return getSize ## Name (set->table);																\
	}																																			\
																																				\
	bool isEmpty ## Name(HashSet ## Name * set) {													\
		return getSize ## Name(set)==0;																			\
	}																																			\
																																				\
	HSIterator ## Name * iterator ## Name(HashSet ## Name * set) {				\
		return allocHSIterator ## Name(set->list, this);										\
	}	
#endif
