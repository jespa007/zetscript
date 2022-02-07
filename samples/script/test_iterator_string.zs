var sum_k=0;
var sum_v=0;
var str="abcd"

for(var it=str.iter();!it.end();it++){
	var k,v;
	v,k=it.get();
	sum_k+=k;
	sum_v+=v;
	
	Console::outln("k:{0} v:{1} ",k,v);
}

System::assert(sum_k==6,"sum_k!=6")
System::assert(sum_v==str,"sum_v!="+str)

