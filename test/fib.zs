// mingw gcc best timing 1220ms - 1250ms
// vc++      best timing 1600ms - 1650ms
function fibR(n)
{
    if (n < 2) {
     	return n; 
    }
    return fibR(n-2)+fibR(n-1);
}

var start=System.clock();
print("fib: " + fibR(34) +" time:"+(System.clock()-start)+"ms\n");

