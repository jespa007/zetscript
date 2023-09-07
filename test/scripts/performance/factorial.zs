function factorial(n){
    if (n == 0) {
        return 1;
    }else{
        return n * factorial(n - 1);
    }
}

for(var i=0; i < 10; i++){
	var start=System::clock();
	Console::outln("fib: {0} time: {1},fibR(34),System::clock()-start);
}
