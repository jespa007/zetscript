var v1=[1,"string",true]
var sum_k=0
var sum_v=""
for(var v in v1){
	sum_v+=v
}

System::assert(sum_v=="1stringtrue","sum_v!=1stringtrue")

sum_k=0
sum_v=""

for(var k,v in v1){
	sum_k+=k
	sum_v+=v
}

System::assert(sum_k==3,"sum_k!=3")
System::assert(sum_v=="1stringtrue","sum_v!=1stringtrue")
