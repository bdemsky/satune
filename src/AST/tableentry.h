/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   tableentry.h
 * Author: hamed
 *
 * Created on June 16, 2017, 3:54 PM
 */

#ifndef TABLEENTRY_H
#define TABLEENTRY_H

#include "classlist.h"
#include "mymemory.h"
struct TableEntry {
	uint64_t output;
	uint64_t inputs[];
};

TableEntry* allocTableEntry(uint64_t* inputs, uint inputSize, uint64_t result);
void deleteTableEntry(TableEntry* tableEntry);

#endif/* TABLEENTRY_H */

