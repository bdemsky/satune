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
#include <string.h>

IncrementalSolver *allocIncrementalSolver() {
	IncrementalSolver *This = (IncrementalSolver *)ourmalloc(sizeof(IncrementalSolver));
	This->buffer = ((int *)ourmalloc(sizeof(int) * IS_BUFFERSIZE));
	This->solution = NULL;
	This->solutionsize = 0;
	This->offset = 0;
	This->timeout = NOTIMEOUT;
	createSolver(This);
	return This;
}

void deleteIncrementalSolver(IncrementalSolver *This) {
	killSolver(This);
	ourfree(This->buffer);
	if (This->solution != NULL)
		ourfree(This->solution);
	ourfree(This);
}

void resetSolver(IncrementalSolver *This) {
	killSolver(This);
	This->offset = 0;
	createSolver(This);
}

void addClauseLiteral(IncrementalSolver *This, int literal) {
	This->buffer[This->offset++] = literal;
	if (This->offset == IS_BUFFERSIZE) {
		flushBufferSolver(This);
	}
}

void addArrayClauseLiteral(IncrementalSolver *This, uint numliterals, int *literals) {
	uint index = 0;
	while (true) {
		uint bufferspace = IS_BUFFERSIZE - This->offset;
		uint numtowrite = numliterals - index;
		if (bufferspace > numtowrite) {
			memcpy(&This->buffer[This->offset], &literals[index], numtowrite * sizeof(int));
			This->offset += numtowrite;
			This->buffer[This->offset++] = 0;	//have one extra spot always
			if (This->offset == IS_BUFFERSIZE) {//Check if full
				flushBufferSolver(This);
			}
			return;
		} else {
			memcpy(&This->buffer[This->offset], &literals[index], bufferspace * sizeof(int));
			This->offset += bufferspace;
			index += bufferspace;
			flushBufferSolver(This);
		}
	}
}

void finishedClauses(IncrementalSolver *This) {
	This->buffer[This->offset++] = 0;
	if (This->offset == IS_BUFFERSIZE) {
		flushBufferSolver(This);
	}
}

void freeze(IncrementalSolver *This, int variable) {
	addClauseLiteral(This, IS_FREEZE);
	addClauseLiteral(This, variable);
}

int solve(IncrementalSolver *This) {
	//add an empty clause
	startSolve(This);
	return getSolution(This);
}

void startSolve(IncrementalSolver *This) {
	addClauseLiteral(This, IS_RUNSOLVER);
	flushBufferSolver(This);
}

int getSolution(IncrementalSolver *This) {
	int result = readStatus(This);
	if (result == IS_SAT) {
		int numVars = readIntSolver(This);
		if (numVars > This->solutionsize) {
			if (This->solution != NULL)
				ourfree(This->solution);
			This->solution = (int *) ourmalloc((numVars + 1) * sizeof(int));
			This->solution[0] = 0;
		}
		readSolver(This, &This->solution[1], numVars * sizeof(int));
		This->solutionsize = numVars;
	} else if (result == IS_INDETER){
		return result;	
	} else {//Reading unsat explanation
		int numVars = readIntSolver(This);
		if (numVars > This->solutionsize) {
			if (This->solution != NULL)
				ourfree(This->solution);
			This->solution = (int *) ourmalloc((numVars + 1) * sizeof(int));
			This->solution[0] = 0;
		}
		readSolver(This, &This->solution[1], numVars * sizeof(int));
		This->solutionsize = numVars;
	}
	return result;
}

int readIntSolver(IncrementalSolver *This) {
	int value;
	readSolver(This, &value, 4);
	return value;
}

int readStatus(IncrementalSolver *This) {
	int retval;
	fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET(This->from_solver_fd, &rfds);
	fd_set *temp;
	if (This->timeout == NOTIMEOUT) {
		retval = select(This->from_solver_fd + 1, &rfds, NULL, NULL, NULL);
	} else {
		struct timeval tv;
		tv.tv_sec = This->timeout;
		tv.tv_usec = 0;
		retval = select(This->from_solver_fd + 1, &rfds, NULL, NULL, &tv);
	}
	if (retval == -1) {
		perror("Error in select()");
		exit(EXIT_FAILURE);
	}
	else if (retval) {
		printf("Data is available now.\n");
		return readIntSolver(This);
	} else {
		printf("Timeout for the solver\n");
		return IS_INDETER;
	}
}

void readSolver(IncrementalSolver *This, void *tmp, ssize_t size) {
	char *result = (char *) tmp;
	ssize_t bytestoread = size;
	ssize_t bytesread = 0;
	do {
		ssize_t n = read(This->from_solver_fd, &((char *)result)[bytesread], bytestoread);
		if (n == -1) {
			model_print("Read failure\n");
			exit(-1);
		}
		bytestoread -= n;
		bytesread += n;
	} while (bytestoread != 0);
}

bool getValueSolver(IncrementalSolver *This, int variable) {
	return This->solution[variable];
}

void createSolver(IncrementalSolver *This) {
	int to_pipe[2];
	int from_pipe[2];
	if (pipe(to_pipe) || pipe(from_pipe)) {
		model_print("Error creating pipe.\n");
		exit(-1);
	}
	if ((This->solver_pid = fork()) == -1) {
		model_print("Error forking.\n");
		exit(-1);
	}
	if (This->solver_pid == 0) {
		//Solver process
		close(to_pipe[1]);
		close(from_pipe[0]);
		int fd = open("log_file", O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);

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
		This->to_solver_fd = to_pipe[1];
		This->from_solver_fd = from_pipe[0];
		close(to_pipe[0]);
		close(from_pipe[1]);
	}
}

void killSolver(IncrementalSolver *This) {
	close(This->to_solver_fd);
	close(This->from_solver_fd);
	//Stop the solver
	if (This->solver_pid > 0) {
		int status;
		kill(This->solver_pid, SIGKILL);
		waitpid(This->solver_pid, &status, 0);
	}
}

//DEBUGGING CODE STARTS
bool first = true;
//DEBUGGING CODE ENDS

void flushBufferSolver(IncrementalSolver *This) {
	ssize_t bytestowrite = sizeof(int) * This->offset;
	ssize_t byteswritten = 0;
#ifdef CONFIG_DEBUG
	for (uint i = 0; i < This->offset; i++) {
		if (first)
			printf("(");
		if (This->buffer[i] == 0) {
			printf(")\n");
			first = true;
		} else {
			if (!first)
				printf(" + ");
			first = false;
			printf("%d", This->buffer[i]);
		}
	}
#endif
	do {
		ssize_t n = write(This->to_solver_fd, &((char *)This->buffer)[byteswritten], bytestowrite);
		if (n == -1) {
			perror("Write failure\n");
			model_print("to_solver_fd=%d\n",This->to_solver_fd);
			exit(-1);
		}
		bytestowrite -= n;
		byteswritten += n;
	} while (bytestowrite != 0);
	This->offset = 0;
}
