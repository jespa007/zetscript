// Laptop N5050 Intel(R) Core(TM) i5-2450M CPU @ 2.50GHz
// mingw gcc best timing 1414ms
// vc++      best timing 1767ms
function fibR(n) {
	if (n < 2) {
     	return n 
    }
    return fibR(n-2)+fibR(n-1)
}

for(var i=0; i <10; i++)
{
	var start=System::clock()
	Console::outln("fib: {0} time: {1} ms", fibR(34),System::clock()-start);
}



