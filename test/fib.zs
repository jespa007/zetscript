function fibR(n)
{
    if (n < 2) {
     	return n; 
    }
    return fibR(n-2)+fibR(n-1);
}

var start=IO.clock();
print("start:"+start);
var f=fibR(34);
var end=IO.clock();
print("end:"+end);
print("fib: " + f +" time:"+(end-start)+"ms");

