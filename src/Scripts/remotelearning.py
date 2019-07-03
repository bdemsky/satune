import re
import argparse
import sys
from threading import Thread
import subprocess
import os

# 1) Deploy on all the servers
# 2) Based on the benchmark selection, run each learning set on a server
# 3) After being done with that, it sshould calculate the first best 3
# 4) Run them indivisually
# 5) Generate the excel sheet!
SRCDIR="/scratch/hamed/constraint_compiler/src"
LOCALSRCDIR="/scratch/satcheck/satproject/constraint_compiler/src"
class ArgParser:
    def __init__(self):
        self.parser = argparse.ArgumentParser(description='Parsing the output log of the CSolver auto tuner ...')
        self.parser.add_argument('--bench', '-b', metavar='sudoku', type=str, nargs=1,help='Benchmark that you want to learn on')
        self.args = self.parser.parse_args()
        
    def getBenchmarkName(self):
        return self.args.bench[0]
       
def deploy():
	os.system("cd ../; ./deploy-cs.sh")

def getServerNeeded(benchmark):
	variable = ""
	with open("./Benchmarks/" + benchmark + "/learn.sh") as f:
		line = f.readline()
		while "declare -a LearningSet=" not in line:
			line = f.readline()
		while ")" not in line:
			variable = variable + line
			line = f.readline()
		variable = variable + line
	return variable.count("\"")/2

def getAvailableServerList(needed):
	global SRCDIR
	available = []
	for i in range(4,12):
		print ("Checking availability for server " + str(i))
		HOST="dc-"+ str(i) + ".calit2.uci.edu"
		COMMAND="cd "+SRCDIR+"; python ./Scripts/serverstatus.py"
		ssh = subprocess.Popen(["ssh", "%s" % HOST, COMMAND],
			shell=False,
			stdout=subprocess.PIPE,
			stderr=subprocess.PIPE)
		result = ssh.stdout.readlines()
		if result == []:
			error = ssh.stderr.readlines()
			print >>sys.stderr, "ERROR: %s" % error
		else:
			print ("Result of running serverStatus: ")
			print result
			if "AVAILABLE\n" in result:
				available.append(i)
				if len(available) >= needed:
					break
	return available

def startLearningProcess(benchmark, server, learningSet):
	global SRCDIR
	HOST="dc-"+ str(server) + ".calit2.uci.edu"
	ALGORITHM = "2"
	LOGFILE= benchmark + "-" + str(learningSet) + ".log"
	print("Running benchmark " + benchmark + "(Set="+ str(learningSet)+") on server")
	COMMAND=("cd "+SRCDIR+"; ./Scripts/learnresultgen.sh " +
		benchmark + " " + str(learningSet) + " " + ALGORITHM + " &> " + LOGFILE + "; mv *.csv ~/; echo 'SUCCESS'")
	print("Calling the following command:\n" + COMMAND)
	ssh = subprocess.Popen(["ssh", "%s" % HOST, COMMAND],
		shell=False,
		stdout=subprocess.PIPE,
		stderr=subprocess.PIPE)
	result = ssh.stdout.readlines()
	if result == []:
		error = ssh.stderr.readlines()
		print >>sys.stderr, "ERROR: %s" % error
	else:
		print ("Result of running serverStatus: ") 
		print result

def moveCSVFiles():
	global LOCALSRCDIR
	os.system("mv ~/*.csv "+ LOCALSRCDIR)


def main():
	benchmark = ArgParser().getBenchmarkName()
#	print("Deploying on all the servers ...")
#	deploy()
	serverNumber = getServerNeeded(benchmark)
	print("Learning on " + benchmark + " needs " + str(serverNumber) + " servers.")
	availableServers = getAvailableServerList(serverNumber)
	print ("Available Server:" + str(availableServers))
	if serverNumber > len(availableServers):
		print("Servers are busy. We don't have enough server available for learning ...")
		sys.exit(1)
	try:
		threads = []
		for i in range(serverNumber):
			t = Thread(target=startLearningProcess, args=(benchmark, availableServers[i], i, ))
			t.start()
			threads.append(t)
		
		for t in threads:
			t.join()
		moveCSVFiles()
	except:
		print("Exception in creating learning thread ...")
		sys.exit(1)



if __name__ == "__main__":
	main()
