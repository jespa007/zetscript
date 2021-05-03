var o={a:1;b:2}
var k,v;
Console::outln("size:"+o.size());
for(var it=o.iter();!it.end();it._next()){
	k,v=it.get();
	Console::outln("k:{0} v:{1} ",k,v);
}