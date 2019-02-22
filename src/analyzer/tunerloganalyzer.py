import re
import argparse
import sys
import os
import plot as pl

class AutoTunerArgParser:
	def __init__(self):
		self.parser = argparse.ArgumentParser(description='Parsing the output log of the CSolver auto tuner ...')
		self.parser.add_argument('--folder', '-f', metavar='bin', type=str, nargs=1,help='output log of running the autotuner ...')
		self.args = self.parser.parse_args()

	def getFolder(self):
		return self.args.folder[0]

PROBLEMS = []

TUNABLEHEADER = ["DECOMPOSEORDER", "MUSTREACHGLOBAL", "MUSTREACHLOCAL", "MUSTREACHPRUNE", "OPTIMIZEORDERSTRUCTURE",
                "ORDERINTEGERENCODING", "PREPROCESS", "NODEENCODING", "EDGEENCODING", "MUSTEDGEPRUNE", "ELEMENTOPT",
                "ENCODINGGRAPHOPT", "ELEMENTOPTSETS", "PROXYVARIABLE", "MUSTVALUE", "NAIVEENCODER", "VARIABLEORDER",
                "PROBLEM","SATTIME", "EXECTIME","TUNERNUMBER"]

configs = {"EXECTIME": "-",
		"SATTIME":"-",
		"TESTCASE":"-",
		"PREPROCESS" : "-",
		"ELEMENTOPT" : "-",
		"ELEMENTOPTSETS" : "-",
		"PROXYVARIABLE" : "-",
		"#SubGraph" : "-",
		"NODEENCODING" : "-",
		"EDGEENCODING" : "-",
		"NAIVEENCODER" :"-",
		"ENCODINGGRAPHOPT" : "-"
		}

REGEXES = {"EXECTIME": "CSOLVER solve time: (.*)",
		"SATTIME":"SAT Solving time: (.*)",
		"TESTCASE": "deserializing (.+) ...",
		"PREPROCESS" : "Param PREPROCESS = (.*)range=\[0,1\]",
		"ELEMENTOPT" : "Param ELEMENTOPT = (.*)range=\[0,1\]",
		"ELEMENTOPTSETS" : "Param ELEMENTOPTSETS = (.*)range=\[0,1\]",
		"PROXYVARIABLE" : "Param PROXYVARIABLE = (.*)range=\[1,5\]",
		"#SubGraph" : "#SubGraph = (.*)",
		"NODEENCODING" : "Param NODEENCODING = (.*)range=\[0,3\](.*)",
		"EDGEENCODING" : "Param EDGEENCODING = (.*)range=\[0,2\](.*)",
		"NAIVEENCODER" : "Param NAIVEENCODER = (.*)range=\[1,3\](.*)",
		"ENCODINGGRAPHOPT" : "Param ENCODINGGRAPHOPT = (.*)range=\[0,1\]"
		}

def printHeader(file):
	global TUNABLEHEADER
	mystr=""
	for header in TUNABLEHEADER:
		 mystr+=str(header)+","
	file.write(mystr)
	file.write("\n")

def dump(file, row):
	global TUNABLEHEADER
	mystr=""
	data = []
	for i in range(len(TUNABLEHEADER)):
		mystr += row[TUNABLEHEADER[i]]+ ","
		data.append(row[TUNABLEHEADER[i]])
	print ("mystr is:"+ mystr)
	file.write(mystr)
	file.write("\n")
	return data

def loadTunerInfo(row, filename):
	with open(filename) as f:
		for line in f:
			numbers = re.findall('\d+',line)
			numbers = list(map(int,numbers))
			row[TUNABLEHEADER[numbers[3]]] = row[TUNABLEHEADER[numbers[3]]] + str(numbers[7])

def loadSolverTime(row, filename):
	global REGEXES
	global configs
	with open(filename) as f:
		for line in f:
			for regex in REGEXES:
				p = re.compile(REGEXES[regex])
				token = p.search(line)
				if token is not None:
					if regex == "TESTCASE":
						configs[regex] = re.search(REGEXES[regex], line).group(1)
					else:
						configs[regex] = re.findall("\d+\.?\d*", line)[0]
	row["SATTIME"] = configs["SATTIME"]
	row["EXECTIME"] = configs["EXECTIME"]

def loadProblemName(row,filename):
	global PROBLEMS
	with open(filename) as f:
		problem = f.readline().replace("\n","")
		probNumber = int(f.readline())
		if probNumber >= len(PROBLEMS):
			PROBLEMS.insert(probNumber,problem)
		elif PROBLEMS[probNumber] != problem:
			PROBLEMS[probNumber] = problem
		row["PROBLEM"] = problem

def loadTunerNumber(row, filename):
	with open(filename) as f:
		row["TUNERNUMBER"] = f.readline().replace("\n","")
def analyzeLogs(file):
	global configs
	argprocess = AutoTunerArgParser()
	printHeader(file)
	rows = []
	data = []
	i = 0
	while True :
		row = {"DECOMPOSEORDER" : "",
			"MUSTREACHGLOBAL" : "",
			"MUSTREACHLOCAL" : "",
			"MUSTREACHPRUNE" : "", 
			"OPTIMIZEORDERSTRUCTURE" : "",
			"ORDERINTEGERENCODING" : "",
			"PREPROCESS" : "",
			"NODEENCODING" : "",
			"EDGEENCODING" : "",
			"MUSTEDGEPRUNE" : "",
			"ELEMENTOPT" : "",
			"ENCODINGGRAPHOPT" : "", 
			"ELEMENTOPTSETS" : "", 
			"PROXYVARIABLE" : "", 
			"MUSTVALUE" : "", 
			"NAIVEENCODER" : "", 
			"VARIABLEORDER" : "",
			"PROBLEM":"",
			"SATTIME":"",
			"EXECTIME": "",
			"TUNERNUMBER":""
		}
		try:
			loadTunerNumber(row, argprocess.getFolder() + "/tunernum" + str(i))
			loadTunerInfo(row, argprocess.getFolder()+"/tuner"+str(i)+"used")
			loadSolverTime(row, argprocess.getFolder()+"/log"+str(i))
			loadProblemName(row, argprocess.getFolder()+"/problem"+str(i))
			data.append(dump(file, row))
			rows.append(row)
		except IOError:
			break
		i += 1
	return rows, data

def tunerCountAnalysis(file, rows):
	global TUNABLEHEADER
	global PROBLEMS
	tunercount = {}
	tunernumber = {}
	for row in rows:
		mystr=""
		for i in range(18):
			if not row[TUNABLEHEADER[i]]:
				mystr += "."
			else:
				mystr+=row[TUNABLEHEADER[i]]
		if mystr not in tunercount:
			tunercount.update({mystr : 1})
			tunernumber.update({mystr : str(row["TUNERNUMBER"])})
		else :
			tunercount[mystr] += 1
			tunernumber[mystr] += "-" + str(row["TUNERNUMBER"])

	problems = set(map(lambda x: x["PROBLEM"], rows))
	print ("Number of repititive tuners")
	for key in tunercount:
		if tunercount[key] > 1:
			print( key + "(ids:" + tunernumber[key]  + ") = #" + str(tunercount[key]) )

def combineRowForEachTuner(rows):
	global PROBLEMS
	newRows = []
	combined = None
	for row in rows:
		if row["PROBLEM"] == PROBLEMS[0]:
			combined = row
		for key in row:
			if row[key]:
				combined[key] = row[key]
		if row["PROBLEM"] == PROBLEMS[len(PROBLEMS)-1]:
			newRows.append(combined)
	return newRows

def transformDataset(rows):
	print(rows)


def main():
	global TUNABLEHEADER
	file = open("tuner.csv", "w")
	rows, data = analyzeLogs(file)
	tunerCountAnalysis(file, combineRowForEachTuner(rows) )
	file.close()
	#transformDataset(data)
	pl.plot(data, TUNABLEHEADER)


if __name__ == "__main__":
	main()
