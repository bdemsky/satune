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

class IncrementalSolver {
public:
	IncrementalSolver();
	~IncrementalSolver();
	void addClauseLiteral(int literal);
	void finishedClauses();
	void freeze(int variable);
	int solve();
	void startSolve();
	int getSolution();

	bool getValue(int variable);
	void reset();
	MEMALLOC;

private:
	void createSolver();
	void killSolver();
	void flushBuffer();
	int readIntSolver();
	void readSolver(void * buffer, ssize_t size);
	int * buffer;
	int * solution;
	int solutionsize;
	int offset;
	pid_t solver_pid;
	int to_solver_fd;
	int from_solver_fd;
};
#endif
