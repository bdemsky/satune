#!/bin/bash

#Terminate the script if even one command fails
set -e

BASE=../
SERVERS="dc-8.calit2.uci.edu dc-9.calit2.uci.edu dc-10.calit2.uci.edu dc-11.calit2.uci.edu"
REMOTEDIR="/scratch/hamed/"
INFILE="constraint_compiler/"
SRC="constraint_compiler/src/"
OUTFILE=csolver.tar.gz
USER=hamed

cd $BASE

rm -f $OUTFILE
tar -czvf $OUTFILE $INFILE

for SERVER in $SERVERS; do
	scp $OUTFILE "$USER@$SERVER:$REMOTEDIR"
	ssh $USER@$SERVER "cd $REMOTEDIR; sudo rm -r $SRC; tar -xzvf $OUTFILE; cd $SRC; make clean; ./setup.sh"
done