// Laptop N5050 Intel(R) Core(TM) i5-2450M CPU @ 2.50GHz
// mingw gcc best timing 1900ms - 2000ms
// vc++      best timing 2447ms - 2500ms
function fibR(n) {
	if (n < 2) {
     	return n; 
    }
    return fibR(n-2)+fibR(n-1);
}

var start=System::clock();
System::println("fib: {0} time: {1} ms", fibR(34),System::clock()-start);
