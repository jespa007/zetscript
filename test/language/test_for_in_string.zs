var str="abcd"
var sum_k=0
var sum_v=0

for(var v in str){
	sum_v+=v
}

System::assert(sum_v=='a'+'b'+'c'+'d',"sum_v!="+str)

sum_k=0
sum_v=0

for(var k,v in str){
	sum_k+=k
	sum_v+=v
}

System::assert(sum_k==6,"sum_k!=6")
System::assert(sum_v=='a'+'b'+'c'+'d',"sum_v!="+str)

