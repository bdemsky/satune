/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Type.h
 * Author: hamed
 *
 * Created on June 13, 2017, 1:33 PM
 */

#ifndef TYPE_H
#define TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long int uint64;
typedef uint64 Type;    

struct Element{
    uint64 value;
};

typedef struct Element ELEMENT;
#ifdef __cplusplus
}
#endif

#endif /* TYPE_H */

