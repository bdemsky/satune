/*      Copyright (c) 2015 Regents of the University of California
 *
 *      Author: Brian Demsky <bdemsky@uci.edu>
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      version 2 as published by the Free Software Foundation.
 */

#ifndef INC_SOLVER_H
#define INC_SOLVER_H
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "solver_interface.h"
#include "classlist.h"

#define NOTIMEOUT -1

struct IncrementalSolver {
	int *buffer;
	int *solution;
	int solutionsize;
	uint offset;
	pid_t solver_pid;
	int to_solver_fd;
	int from_solver_fd;
	long timeout;
};

IncrementalSolver *allocIncrementalSolver();
void deleteIncrementalSolver(IncrementalSolver *This);
void addClauseLiteral(IncrementalSolver *This, int literal);
void addArrayClauseLiteral(IncrementalSolver *This, uint numliterals, int *literals);
void finishedClauses(IncrementalSolver *This);
void freeze(IncrementalSolver *This, int variable);
int solve(IncrementalSolver *This);
void startSolve(IncrementalSolver *This);
int getSolution(IncrementalSolver *This);
bool getValueSolver(IncrementalSolver *This, int variable);
void resetSolver(IncrementalSolver *This);
void createSolver(IncrementalSolver *This);
void killSolver(IncrementalSolver *This);
void flushBufferSolver(IncrementalSolver *This);
int readIntSolver(IncrementalSolver *This);
int readStatus(IncrementalSolver *This);
void readSolver(IncrementalSolver *This, void *buffer, ssize_t size);
#endif
