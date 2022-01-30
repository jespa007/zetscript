var sum_k=0;
var sum_v=0;
for(var it="abcd".iter();!it.end();it++){
	var k,v;
	v,k=it.get();
	Console::outln("k:{0} v:{1} ",k,v);
	sum_k+=k;
	sum_v+=v;
}

var sum_v_expected='a'+'b'+'c'+'d'

System::assert(sum_k==6,"sum_k!=6")
System::assert(sum_v==sum_v_expected,"sum_v!="+sum_v_expected)

