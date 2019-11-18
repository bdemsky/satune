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
        echo "./runinterpreter.sh [benchmark] [--alloy/--z3/--smtrat/--mathsat/--all] [timeout]"
        exit 1
fi


BIN=./bin
OUTPUT="$1-SMTsolvers.csv"
DUMP=$(find . -name "*.dump")
cd $BIN
LINE="Benchmark, Alloy, Z3, Smtrat, Mathsat"
echo $Line > $OUTPUT
for d in $DUMP; do
	if [[ $d = *$1* ]] && [[ $d = *learningset* ]]; then
		echo $d
		if [[ $2 = *--all* ]]; then
			SOLVERS="--alloy --z3 --smtrat --mathsat"
			LINE=""
			for solver in $SOLVERS; do
				START=$(date +%s.%N)
				echo "./run.sh deserializealloytest .$d $solver $3"
                        	./run.sh deserializealloytest "."$d $solver $3
                        	END=$(date +%s.%N)
                        	DIFF=$(echo "$END - $START" | bc)
				LINE="$LINE,$DIFF"
			done
			echo "$d$LINE" >> $OUTPUT
		else
			START=$(date +%s.%N)
			./run.sh deserializealloytest "."$d $2 $3
			END=$(date +%s.%N)
			DIFF=$(echo "$END - $START" | bc)
			echo "CSOLVER solve time: $DIFF"
			cat solution.sol
			echo "Best tuner"
		fi
		
	fi
done
