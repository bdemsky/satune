import psutil
from time import sleep
# gives a single float value
avg = 0.0
count = 0
for i in range(350):
	avg = (psutil.cpu_percent() + avg*count)/(count+1)
	count = count + 1
	sleep(0.1)


if avg> 15:
	print "BUSY"
else:
	print "AVAILABLE"
