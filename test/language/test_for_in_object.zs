var o={a:1,b:"string",c:true}
var sum_k="",sum_v="";

for(var v in o){
	sum_v+=v
}

System::assert(sum_v=="1stringtrue","sum_v!=1stringtrue")


sum_k=""
sum_v="";

for(var k,v in o){
	sum_k+=k
	sum_v+=v
}

System::assert(sum_k=="abc","sum_k!=abc")
System::assert(sum_v=="1stringtrue","sum_v!=1stringtrue")

