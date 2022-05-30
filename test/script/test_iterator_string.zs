var sum_k=0;
var sum_v=0;
var str="abcd"

for(var it=str.iter();!it.end();it++){
	var k,v;
	k,v=it.get();
	sum_k+=k;
	sum_v+=v;
	
}

System::assert(sum_k==6,"sum_k!=6")
System::assert(sum_v=='a'+'b'+'c'+'d',"sum_v!="+str)

