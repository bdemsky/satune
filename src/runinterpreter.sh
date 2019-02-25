#!/bin/bash
# run as the following:
# ./runalloy.sh [hexiom] [--alloy]
# ./runalloy.sh [nqueens] [--alloy]
# ./runalloy.sh [sudoku-csolver] [--alloy]
# ./runalloy.sh [killerSudoku] [--alloy]

#./run.sh deserializealloytest ../Benchmarks/sudoku-csolver/4x4.dump --alloy
#./run.sh java edu.mit.csail.sdg.alloy4whole.ExampleAlloyCompilerNoViz satune.als > solution.log

if [ "$#" -lt 2 ]; then
        echo "Illegal number of argument"
        echo "./runinterpreter.sh [benchmark] [--alloy/--z3/--smtrat/--mathsat] [timeout]"
        exit 1
fi


BIN=./bin
DUMP=$(find . -name "*.dump")
cd $BIN
for d in $DUMP; do
	if [[ $d = *$1* ]]; then
		echo $d
		START=$(date +%s.%N)
		./run.sh deserializealloytest "."$d $2 $3
		END=$(date +%s.%N)
		DIFF=$(echo "$END - $START" | bc)
		echo "CSOLVER solve time: $DIFF"
		cat solution.sol
		echo "Best tuner"
	fi
done
