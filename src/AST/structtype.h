/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   structtype.h
 * Author: hamed
 *
 * Created on June 21, 2017, 10:37 AM
 */

#ifndef STRUCTTYPE_H
#define STRUCTTYPE_H
#include "ops.h"
#include "structs.h"
#define GETSTRUCTTYPE(s) (((Struct*)s)->stype)
#define GETPARENTSVECTOR(s) (((Struct*)s)->parents)
#define ADDNEWPARENT(obj,par) pushVectorVoid(((Struct*)obj)->parents,(void*) par)
#define ADDNEWPARENTTOOBJECTARRAY(array,size,par) \
	do{	\
		for(int i=0; i<size; i++){	\
			ADDNEWPARENT(array[i], par);	\
		}	\
	}while(0)
#define DELETEPARENTSVECTOR(obj) deleteVectorArrayVoid(((Struct*)obj)->parents)

struct Struct {
	StructType stype;
	VectorVoid* parents;
};

#endif /* STRUCTTYPE_H */

