var test_sum=0;
// for
Console::outln("test for:");
for(var i=0;i<20;i++){ 
	if(i==1) { 
		continue;
	}
	
	if(i==10){
		break;
	}
	
	Console::outln("i:"+i);
	
	test_sum+=i;
}


// while
Console::outln("test while:");
test_sum=0
var j=0;
while(j < 10){
	
	j++;
	
	if(j==1) { 
		continue;
	}
	
	if(j==10){
		break;
	}
	
	Console::outln("j:"+j);
	test_sum+=j;
}


//do-while
test_sum=0
Console::outln("test do-while:");
j=0;
do{
	j++;
	
	if(j==1) { 
		continue;
	}
	
	if(j==10){
		break;
	}
	Console::outln("j:"+j);
	test_sum+=j;
}while(j<20);


//test double for i/k
test_sum=0
for(var i=0;i<5;i++){
	
	if(i==0) { 
		continue;
	}
	
	if(i==3){
		break;
	}

	
	for(var k=0;k<10;k++){ 
		if(k==1) { 
			continue;
		}
		
		if(k==5){
			break;
		}
		
		Console::outln("i:{0} k:{1}",i,k);
		test_sum+=k*i;
	}
	//Console::outln("i:"+i);
}

