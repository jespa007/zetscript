function fibR(n)
{
    if (n < 2) { 
	return n; 
    }
    var t1=fibR(n-2);
     var t2=fibR(n-1);

    return t1+t2;
}


//var N = 34;
var N = 34; // 
var M = N; //Should return 5702887
print("fib: " + (fibR(M)) );
