var str="abcd"
var sum_k=0
var sum_v=""

for(var v in str){
	sum_v+=v
	Console::outln("v:{0}",v);
}

System::assert(sum_v==str,"1. sum_v!="+str)


for(var k,v in str){
	sum_k+=k
	sum_v+=v
	Console::outln("str[{0}]:{1}",k,v);
}

System::assert(sum_k==6,"2.1 sum_k!=6")
System::assert(sum_v==str,"2.2 sum_v!="+str)

