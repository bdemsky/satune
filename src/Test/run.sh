#!/bin/bash

export LD_LIBRARY_PATH=../bin
# For Mac OSX
export DYLD_LIBRARY_PATH=../bin

$@
