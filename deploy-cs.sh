#!/bin/bash

#Terminate the script if even one command fails
set -e

BASE=../
SERVERS="dc-4.calit2.uci.edu dc-5.calit2.uci.edu dc-6.calit2.uci.edu dc-7.calit2.uci.edu dc-8.calit2.uci.edu dc-9.calit2.uci.edu dc-10.calit2.uci.edu dc-11.calit2.uci.edu"
#SERVERS="dc-1.calit2.uci.edu dc-2.calit2.uci.edu dc-3.calit2.uci.edu"
REMOTEDIR="/scratch/hamed/"
INFILE="constraint_compiler/"
SRC="constraint_compiler/src/"
SHAREDDIR=~/
OUTFILE=csolver.tar.gz
USER=hamed
BIN=${REMOTEDIR}${SRC}/bin
cd $BASE

rm -f $OUTFILE
tar -czvf $OUTFILE $INFILE

cp $OUTFILE $SHAREDDIR
for SERVER in $SERVERS; do
	ssh $USER@$SERVER "cp $SHAREDDIR$OUTFILE $REMOTEDIR; cd $REMOTEDIR; sudo rm -r $SRC; tar -xzvf $OUTFILE; cd $SRC; make clean; ./setup.sh; find -iname csolver -exec rm '{}' \; -exec ln -s $BIN '{}' \;"
done
