var v1=[1,"string",true]
var r=""
Console::outln("size:"+v1.size());
for(var it=v1.iter();!it.end();it++){
	var k,v;
	v,k=it.get();
	Console::outln("k:{0} v:{1} ",k,v);
	r+=v
}

System::assert(r=="1stringtrue","r!=1stringtrue")