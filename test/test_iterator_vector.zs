var v1=[1,"string",true]
Console::outln("size:"+v1.size());
for(var it=v1.iter();!it.end();it++){
	var k,v;
	k,v=it.get();
	Console::outln("k:{0} v:{1} ",k,v);
}
