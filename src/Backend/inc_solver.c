/*      Copyright (c) 2015 Regents of the University of California
 *
 *      Author: Brian Demsky <bdemsky@uci.edu>
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      version 2 as published by the Free Software Foundation.
 */

#include "inc_solver.h"
#define SATSOLVER "sat_solver"
#include <fcntl.h>
#include "common.h"

IncrementalSolver * allocIncrementalSolver() {
	IncrementalSolver *this=(IncrementalSolver *)ourmalloc(sizeof(IncrementalSolver));
	this->buffer=((int *)ourmalloc(sizeof(int)*IS_BUFFERSIZE));
	this->solution=NULL;
	this->solutionsize=0;
	this->offset=0;
	createSolver(this);
	return this;
}

void deleteIncrementalSolver(IncrementalSolver * this) {
	killSolver(this);
	ourfree(this->buffer);
	if (this->solution != NULL)
		ourfree(this->solution);
}

void resetSolver(IncrementalSolver * this) {
	killSolver(this);
	this->offset = 0;
	createSolver(this);
}

void addClauseLiteral(IncrementalSolver * this, int literal) {
	this->buffer[this->offset++]=literal;
	if (this->offset==IS_BUFFERSIZE) {
		flushBufferSolver(this);
	}
}

void finishedClauses(IncrementalSolver * this) {
	addClauseLiteral(this, 0);
}

void freeze(IncrementalSolver * this, int variable) {
	addClauseLiteral(this, IS_FREEZE);
	addClauseLiteral(this, variable);
}

int solve(IncrementalSolver * this) {
	//add an empty clause
	startSolve(this);
	return getSolution(this);
}


void startSolve(IncrementalSolver *this) {
	addClauseLiteral(this, IS_RUNSOLVER);
	flushBufferSolver(this);
}

int getSolution(IncrementalSolver * this) {
	int result=readIntSolver(this);
	if (result == IS_SAT) {
		int numVars=readIntSolver(this);
		if (numVars > this->solutionsize) {
			if (this->solution != NULL)
				ourfree(this->solution);
			this->solution = (int *) ourmalloc((numVars+1)*sizeof(int));
			this->solution[0] = 0;
		}
		readSolver(this, &this->solution[1], numVars * sizeof(int));
	}
	return result;
}

int readIntSolver(IncrementalSolver * this) {
	int value;
	readSolver(this, &value, 4);
	return value;
}

void readSolver(IncrementalSolver * this, void * tmp, ssize_t size) {
	char *result = (char *) tmp;
	ssize_t bytestoread=size;
	ssize_t bytesread=0;
	do {
		ssize_t n=read(this->from_solver_fd, &((char *)result)[bytesread], bytestoread);
		if (n == -1) {
			model_print("Read failure\n");
			exit(-1);
		}
		bytestoread -= n;
		bytesread += n;
	} while(bytestoread != 0);
}

bool getValueSolver(IncrementalSolver * this, int variable) {
	return this->solution[variable];
}

void createSolver(IncrementalSolver * this) {
	int to_pipe[2];
	int from_pipe[2];
	if (pipe(to_pipe) || pipe(from_pipe)) {
		model_print("Error creating pipe.\n");
		exit(-1);
	}
	if ((this->solver_pid = fork()) == -1) {
		model_print("Error forking.\n");
		exit(-1);
	}
	if (this->solver_pid == 0) {
		//Solver process
		close(to_pipe[1]);
		close(from_pipe[0]);
		int fd=open("log_file", O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU);

		if ((dup2(to_pipe[0], 0) == -1) ||
				(dup2(from_pipe[1], IS_OUT_FD) == -1) ||
				(dup2(fd, 1) == -1)) {
			model_print("Error duplicating pipes\n");
		}
		//    setsid();
		execlp(SATSOLVER, SATSOLVER, NULL);
		model_print("execlp Failed\n");
		close(fd);
	} else {
		//Our process
		this->to_solver_fd = to_pipe[1];
		this->from_solver_fd = from_pipe[0];
		close(to_pipe[0]);
		close(from_pipe[1]);
	}
}

void killSolver(IncrementalSolver * this) {
	close(this->to_solver_fd);
	close(this->from_solver_fd);
	//Stop the solver
	if (this->solver_pid > 0) {
		int status;
		kill(this->solver_pid, SIGKILL);
		waitpid(this->solver_pid, &status, 0);
	}
}

void flushBufferSolver(IncrementalSolver * this) {
	ssize_t bytestowrite=sizeof(int)*this->offset;
	ssize_t byteswritten=0;
	do {
		ssize_t n=write(this->to_solver_fd, &((char *)this->buffer)[byteswritten], bytestowrite);
		if (n == -1) {
			perror("Write failure\n");
			model_print("to_solver_fd=%d\n",this->to_solver_fd);
			exit(-1);
		}
		bytestowrite -= n;
		byteswritten += n;
	} while(bytestowrite != 0);
	this->offset = 0;
}
