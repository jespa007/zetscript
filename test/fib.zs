var a=0;
var b=1;
var c=2;
var d=4;
var e=5;
a-b*5-a-b-c-9/-b-(-d+e);


function fibR(n)
{
    if (n < 2) {
     	return n; 
    }
    return fibR(n-2)+fibR(n-1);
}

print("fib: " + (fibR(1)) );
