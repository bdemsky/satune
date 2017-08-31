/*      Copyright (c) 2015 Regents of the University of California
 *
 *      Author: Brian Demsky <bdemsky@uci.edu>
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      version 2 as published by the Free Software Foundation.
 */

#ifndef SOLVER_INTERFACE_H
#define SOLVER_INTERFACE_H

#define IS_OUT_FD 3

enum SolverResult {IS_UNSAT=0, IS_SAT=1, IS_INDETER=2, IS_FREEZE=3, IS_RUNSOLVER=4};

#define IS_BUFFERSIZE 1024

#endif
