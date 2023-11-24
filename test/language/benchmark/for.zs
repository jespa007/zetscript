function test_sum(){

	var list = []

	for (var i=0; i < 1000000; i++) {
		list.push(i)
	}
	
	var sum = 0
	for (var i in list) {
		sum = sum + i
	}
	
	return sum;
}

for(var i=0; i < 10; i++){
	var start=System::clock();
	Console::outln("sum: {0} elapsed: {1}",test_sum(),(System::clock() - start))
}