function fibR(n) {
	if (n < 2) {
     	return n 
    }
    return fibR(n-2)+fibR(n-1)
}

for(var i=0; i <10; i++)
{
	var start=System::clock()
	Console::outln("fib: {0} time: {1} s", fibR(34),System::clock()-start);
}



