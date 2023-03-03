var v1=[1,"string",true]
var r=""

for(var it=v1._iter();!it._end();it++){
	var k,v;
	k,v=it._get();
	r+=v
}

System::assert(r=="1stringtrue","r!=1stringtrue")