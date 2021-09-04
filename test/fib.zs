// Laptop N5050 Intel(R) Core(TM) i5-2450M CPU @ 2.50GHz
// mingw gcc best timing 1690ms
// vc++      best timing 2000ms
function fibR(n) {
	if (n < 2) {
     	return n 
    }
    return fibR(n-1)+fibR(n-2)
}

for(var i=0; i <10; i++){
	var start=System::clock()
	fibR(34)
	Console::outln(System::clock()-start);
}



