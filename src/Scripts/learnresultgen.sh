#!/bin/bash
# ./learnresultgen.sh [sypet] [learning set = 1, 2, 3, etc.] [algorithm= 1, 2, 3, 4]
set -e

if [ "$#" -lt 3 ]; then
        echo "Illegal number of argument"
        echo "./learnresultgen.sh [sypet] [learning set = 0, 1, 2, 3, etc.] [algorithm = Known Tuner Types: Random Tuner=1, Comp Tuner=2, Kmeans Tuner=3, Simulated Annealing Tuner=4]"
        exit 1
fi

SATUNEDIR=$PWD
BENCHDIR=$SATUNEDIR/Benchmarks/$1
BIN=$SATUNEDIR/bin

source $SATUNEDIR/Benchmarks/common.sh
cd $BENCHDIR
./learn.sh $2 $3
cd $BIN
./run.sh analyzemultituner
cd $SATUNEDIR
TUNERS=$(find "$BIN" -name "*.tuner")
for T in $TUNERS; do
	TUNER=$(basename $T)
	echo "Running tuner "$TUNER
	./Scripts/runbench.sh $1 $TIMEOUT $TUNER &> $BIN/$1"-set"$2"-"$TUNER".log"
	python ./Scripts/autotunerparser.py -f $BIN/$1"-set"$2"-"$TUNER".log"
	mv tuner.csv $1"-set"$2"-"$TUNER".csv"
done

mv ./bin ./"bin-"$1"-set"$2"-alg"$3
