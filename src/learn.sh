#!/bin/bash
# run as the following:
# ./learn.sh [hexiom]
# ./learn.sh [nqueens]
# ./learn.sh [sudoku-csolver]
# ./learn.sh [killerSudoku]



BIN=./bin
DUMP=$(find . -name "*.dump")
cd $BIN
for d in $DUMP; do
	if [[ $d = *$@* ]]; then
		echo $d
		./run.sh deserializerautotune "."$d
	fi
done
