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
		echo "Running: ./run.sh tunerrun "."$d $2 $3 out.out"
		./run.sh tunerrun "."$d $2 $3 out.out
		RETCODE=$?
		echo "Return code: $RETCODE"
		if [ $RETCODE -eq 141 ]; then #Dump info when SAT Solver gets killed by OS ....
			echo "Satune got out of memory"
			echo "deserializing $d ..."
			echo "SAT Solving time: 400000000.0"
			echo "CSOLVER solve time: 400000000.0"
		fi
		echo "Best tuner"
	fi
done
