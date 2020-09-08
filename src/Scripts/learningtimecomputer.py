import os
import sys
import random
import re


path = ''

def validateArgs():
	global path;
	if len(sys.argv) != 2:
		print("Wrong argument! Usage: python learningtimecomputer.py ./bin-sudoku-set0-alg2");
		sys.exit(-1);
	else:
		path = sys.argv[1] + "/";


def computeLearningTime():
	learntime = 0;
	for i in range(10000000):
		logname = path + "log" + str(i);
		timeout = -1;
		if(not os.path.exists(logname)):
			break;
		with open(logname) as fp:
			line = fp.readline()
			timeout = int(line.split()[2])
		
		resultname = path + "result" + str(i)
		if(not os.path.exists(resultname) ):
			learntime += timeout
		else:
			with open(resultname) as fp:
				line = fp.readline()
				learntime += long(line)*1.0/1000000000;

	print("learning time: " + str(learntime*1.0/60) + " min" )
		

def main():
	validateArgs();
	computeLearningTime()

if __name__ == "__main__":
    main()
			    
