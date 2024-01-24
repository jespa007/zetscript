import time

def fib(n):
	if n <= 2:
		return 1
	else:
		return fib(n - 1) + fib(n - 2)


for x in range(1,10):
    start_time = time.time()
    print("fib: %s time: %s s" % (fib(34),time.time() - start_time))
