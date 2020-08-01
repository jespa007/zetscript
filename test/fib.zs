function fibR(n)
{
	var b=0;
    if (n < 2) {
		var i=0;
     	return n; 
    }

    return fibR(n-2)+fibR(n-1);
}

print("fib: " + (fibR(1)) );
