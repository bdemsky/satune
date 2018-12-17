#!/bin/bash

REPORTDIR=./report
PICSDIR=$REPORTDIR/pics


set -e
if [ "$#" -lt 1 ]; then
        echo "Illegal number of argument"
        echo "./analyze.sh [path/to/log/folder]"
        exit 1
fi

mkdir -p $PICSDIR
python tunerloganalyzer.py -f $1
cd $REPORTDIR
make
