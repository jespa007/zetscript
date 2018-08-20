for(var i=0; i < 20; i++){
	
	if((i % 2)==0){
		continue;
	}
	
	print("for1 i:"+i);	
	
}

for(var i=0;;i++){
	
	print("for2 i:"+i);
	{
		if(i==10){
			break;
		}
	}
}

var j=0;

while(j<20){
	
	print("while1 j:"+j);
	
	if(j>10){
		break;
	}
	
	j++;
}

j=0;

do{
	print("while2 j:"+j);
	
	if(j>10){
		break;
	}
	
	j++;
	
}while(j<20);

