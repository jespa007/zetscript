function fibR(n)
{
    if (n < 2) { 
     	return n; 
    }

    return fibR(n-2)+fibR(n-1);
}

print("fib: " + (fibR(1)) );
