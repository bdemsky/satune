import numpy as np
import matplotlib.pyplot as plt
IMAGEDIR = "report/pics/"

def generateHistograms(X, header):
	global IMAGEDIR
	for i, c in enumerate(X.T):
		plt.hist(c)
		plt.savefig(IMAGEDIR + header[i] + "-hist.pdf")
		plt.clf()

def generateScatterPlot(X, Y, header):
	global IMAGEDIR
	for i, feature in enumerate(X.T):
		values = np.unique(feature)
		values = np.sort(values)
		geomean = []
		for value in values:
			a =Y[np.where(feature == value)]
			a = np.array(list(map(lambda x :  x**(1.0/len(a)), a)))
			geomean.append(a.prod())
		plt.plot(feature, Y, 'r.')
		for ii in range(0, len(geomean)-1):
			plt.plot(values[ii:ii + 2], geomean[ii:ii + 2], 'bo-')
		plt.savefig(IMAGEDIR + header[i] + "-scat.pdf")
		plt.clf()

def plot(data, header):
	global IMAGEDIR
	header=header[6:-1]
	data = np.array(data)
	X = data[:, 6:-4]
	X[X==''] = '-1'
	X = X.astype(np.float)
	Y = data[:, -2]
	Y = Y.astype(np.float)
	generateHistograms(X, header)
	generateScatterPlot(X, Y, header)

	


