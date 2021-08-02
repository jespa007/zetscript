
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
}

Console::outln("test while:");
var j=0;
while(j < 10){
	
	j++;
	
	if(j==1) { 
		continue;
	}
	
	if(j==10){
		break;
	}
	
	Console::outln("i:"+j);
	
}

Console::outln("test do-while:");
// do-while
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
}while(j<20);

Console::outln("test for:");
for(var i=0;i<10;i++){
	for(var k=0;k<20;k++){ 
		if(k==1) { 
			continue;
		}
		
		if(k==10){
			break;
		}
		
		Console::outln("i:{0} k:{1}",i,k);
	}
	//Console::outln("i:"+i);
}
