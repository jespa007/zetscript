var test_sum=0;
// for
for(var i=0;i<20;i++){ 
	if(i==1) { 
		continue;
	}
	
	if(i==10){
		break;
	}
	test_sum+=i;
}

System::assert(test_sum==(0+2+3+4+5+6+7+8+9),"1.for-break test_sum!="+(0+2+3+4+5+6+7+8+9))


// while
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

	test_sum+=j;
}

System::assert(test_sum==(0+2+3+4+5+6+7+8+9),"2.while test_sum!="+(0+2+3+4+5+6+7+8+9))


//do-while
test_sum=0
j=0;
do{
	j++;
	
	if(j==1) { 
		continue;
	}
	
	if(j==10){
		break;
	}
	test_sum+=j;
}while(j<20);

System::assert(test_sum==(0+2+3+4+5+6+7+8+9),"3.do-while test_sum!="+(0+2+3+4+5+6+7+8+9))

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
	
		test_sum+=k*i;
	}
}

System::assert(test_sum==(
		(0+2+3+4)*1+
		(0+2+3+4)*2
),"4.double for test_sum!="+(
		(0+2+3+4)*1+
		(0+2+3+4)*2
))

//System::assert(j==10,"3. do-while j!=10")
