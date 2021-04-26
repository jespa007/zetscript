
// for
Console::println("test for:");
for(var i=0;i<20;i++){ 
	if(i==1) { 
		continue;
	}
	
	if(i==10){
		break;
	}
	Console::println("i:"+i);
}

Console::println("test while:");
var j=0;
while(j < 10){
	
	j++;
	
	if(j==1) { 
		continue;
	}
	
	if(j==10){
		break;
	}
	
	Console::println("i:"+j);
	
}

Console::println("test do-while:");
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
	Console::println("j:"+j);
}while(j<20);
