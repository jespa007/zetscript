function test_sum(){

	local list = [];
	
	for (local i=0; i < 1000000; i++) {
		list.push(i);
	}
	
	local sum = 0;
	foreach (i in list) {
		sum = sum + i;
	}
	
	return sum;
}

for(local i=0; i < 10; i++){
	local start=clock();
	print("sum: "+test_sum()+" time: "+(clock()-start)+" s\n");
}



