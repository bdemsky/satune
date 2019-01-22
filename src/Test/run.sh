#!/bin/bash

export CLASSPATH=../bin/original.jar:.:$CLASSPATH
export LD_LIBRARY_PATH=../bin
# For Mac OSX
export DYLD_LIBRARY_PATH=../bin
# For sat_solver
export PATH=.:$PATH
echo $@
$@
