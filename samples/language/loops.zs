
// for
Console::outln("test for:");
for(var i=0,j=0;i < 10; i++,j+=2){ 
	Console::outln("i:"+i+" j:"+j);
}

Console::outln("test while:");
var k=0;
while(k < 10){
	Console::outln("k:"+k++);
}

Console::outln("test do-while:");
k=0;
do{
	Console::outln("k:"+k++);
}while(k<10);