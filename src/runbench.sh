#!/bin/bash
# run as the following:
# ./runbench.sh [hexiom] [timeout] [tuner.conf]
# ./runbench.sh [nqueens] [timeout] [tuner.conf]
# ./runbench.sh [sudoku-csolver] [timeout] [tuner.conf]
# ./runbench.sh [killerSudoku] [timeout] [tuner.conf]

if [ "$#" -lt 3 ]; then
        echo "Illegal number of argument"
        echo "./runbench.sh [benchmark] [timeout] [tuner.conf]"
        exit 1
fi


BIN=./bin
DUMP=$(find . -name "*.dump")
cd $BIN
for d in $DUMP; do
	if [[ $d = *$1* ]]; then
		echo $d
		./run.sh tunerrun "."$d $2 "../"$3 out.out
		echo "Best tuner"
	fi
done
