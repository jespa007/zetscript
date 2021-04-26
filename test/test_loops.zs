
// for
Console::println("test for:");
for(var i=0;i < 10; i++){ 
	Console::println("i:"+i);
}

Console::println("test while:");
var j=0;
while(j < 10){
	Console::println("j:"+j++);
}

Console::println("test do-while:");
j=0;
do{
	Console::println("j:"+j++);
}while(j<10);