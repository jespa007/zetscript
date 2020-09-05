function fibR(n)
{
    if (n < 2) {
     	return n; 
    }
    return fibR(n-2)+fibR(n-1);
}

var start=IO.clock();
print("fib: " + (fibR(34)) +" time:"+(IO.clock()-start));

