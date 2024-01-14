function fibR(n) {
	if (n < 2) {
     	return n;
    }
    return fibR(n-2)+fibR(n-1);
}

for(local i=0; i < 10; i++){
	local start=clock();
	print("fib: "+fibR(34)+" time: "+(clock()-start)+" s\n");
}



