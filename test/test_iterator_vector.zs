var v=[1;"string",true]
var k,v;
Console::outln("size:"+v.size());
for(var it=v.iter();!it.end();it._next()){
	k,v=it.get();
	Console::outln("k:{0} v:{1} ",k,v);
}
