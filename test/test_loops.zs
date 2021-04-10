
// for
System::println("test for:");
for(var i=0;i < 10; i++){ 
	System::println("i:"+i);
}

System::println("test while:");
var j=0;
while(j < 10){
	System::println("j:"+j++);
}

System::println("test do-while:");
j=0;
do{
	System::println("j:"+j++);
}while(j<10);