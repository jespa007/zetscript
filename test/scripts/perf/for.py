from __future__ import print_function

import time

# Map "range" to an efficient range in both Python 2 and 3.
try:
    range = xrange
except NameError:
    pass
    
def test_sum():
	list = []
	for i in range(0, 1000000):
	  list.append(i)
	
	sum = 0
	for i in list:
	  sum += i
	  
	return sum

start = time.process_time()
for i in range(0, 10):
  print("sum: "+str(test_sum())+" elapsed: " + str(time.process_time() - start))


