import re
import argparse
import sys


class AutoTunerArgParser:
    def __init__(self):
        self.parser = argparse.ArgumentParser(description='Parsing the output log of the CSolver auto tuner ...')
        self.parser.add_argument('--file', '-f', metavar='out.log', type=str, nargs=1,help='output log of running the autotuner ...')
        self.args = self.parser.parse_args()
        
    def getFileName(self):
        return self.args.file[0]

HEADER = ["TESTCASE", "SATTIME", "EXECTIME", "PREPROCESS", "ELEMENTOPT", "ELEMENTOPTSETS", "PROXYVARIABLE", "#SubGraph", "NODEENCODING", "EDGEENCODING", "NAIVEENCODER", "ENCODINGGRAPHOPT"]

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
def reorderEntry(entry):
	global HEADER
	result = []
	for key in HEADER:
		result.append(entry[key])
	return result


def printHeader(file):
	global HEADER
	mystr=""
	for key in HEADER:
		 mystr+=key+","
	print >>file, mystr
	
def printConfig(file, data):
	print data
	mystr=""
	for val  in data:
		 mystr+=str(val)+","
	print >> file, mystr


def main():
	global configs
	argprocess = AutoTunerArgParser()
	output = open("tuner.csv", "w")
	printHeader(output)
	result = []
	with open(argprocess.getFileName()) as file:
		for line in file:
			if line.startswith("Mutating"):
				result.append(reorderEntry(configs))
			elif line.startswith("Best tuner"):
				result.append(reorderEntry(configs))
			else :
				for regex in REGEXES:
					p = re.compile(REGEXES[regex])
					token = p.search(line)
					if token is not None:
						if regex == "TESTCASE":
							configs[regex] = re.search(REGEXES[regex], line).group(1)
						else:
							configs[regex] = re.findall("\d+\.?\d*", line)[0]

	#configs["EXECTIME"] = "BEST TUNE:"
	result.append(reorderEntry(configs))
	result.sort(key = lambda entry: entry[0])
	for entry in result:
		printConfig(output, entry)
	print "Done with parsing " + argprocess.getFileName()

if __name__ == "__main__":
	main()
