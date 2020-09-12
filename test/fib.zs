function fibR(n)
{
    if (n < 2) {
     	return n; 
    }
    return fibR(n-2)+fibR(n-1);
}

var start=IO.clock();
var f=fibR(34);
print("fib: " + f +" time:"+(IO.clock()-start)+"ms");

