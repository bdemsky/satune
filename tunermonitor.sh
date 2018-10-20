#!/bin/bash

#Terminate the script if even one command fails
#set -e

SATSOLVER=sat_solver
CSOLVER=runcomptuner
MONITOR=tunermonitor
SLEEPTIME=10
FILE=monitor.log


kill -9 `jobs -ps` &>$FILE

while true; do
	date >> $FILE
	free >> $FILE
	ps aux | grep $SATSOLVER | grep -v "grep" >> $FILE
	ps aux | grep $CSOLVER | grep -v "run.sh" | grep -v "grep" >> $FILE
	ps aux | grep $MONITOR | grep -v "grep" >> $FILE
	echo "*************************" >> $FILE
	sleep $SLEEPTIME
done
