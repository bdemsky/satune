/*      Copyright (c) 2015 Regents of the University of California
 *
 *      Author: Brian Demsky <bdemsky@uci.edu>
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      version 2 as published by the Free Software Foundation.
 */

#ifndef __STL_MODEL_H__
#define __STL_MODEL_H__

#include <vector>
#include <list>
#include "mymemory.h"

template<typename _Tp>
class ModelList : public std::list<_Tp, ModelAlloc<_Tp> >
{
public:
	typedef std::list< _Tp, ModelAlloc<_Tp> > list;

	ModelList() :
		list()
	{ }

	ModelList(size_t n, const _Tp& val = _Tp()) :
		list(n, val)
	{ }

	MEMALLOC;
};

template<typename _Tp>
class ModelVector : public std::vector<_Tp, ModelAlloc<_Tp> >
{
public:
	typedef std::vector< _Tp, ModelAlloc<_Tp> > vector;

	ModelVector() :
		vector()
	{ }

	ModelVector(size_t n, const _Tp& val = _Tp()) :
		vector(n, val)
	{ }

	MEMALLOC;
};

#endif/* __STL_MODEL_H__ */
