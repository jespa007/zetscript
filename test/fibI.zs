function fibR(n)
{
    if (n < 2) { return n; }
    return (fibR(n-2) + fibR(n-1));
}


function fibI(n)
{
    var last;
    var cur;
    var tmp;
    last = 0;
    cur = 1;
    n = n - 1;
    while(n > 0)
    {
        --n;
        tmp = cur;
        cur = last + cur;
        last = tmp;
    }
    return cur;
}


var N = 34;
var M = N; //Should return 433494437
print("fib: " + fibR(M) + "=" + fibI(N) );