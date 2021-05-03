var s="this is an string iterator"
var k,v;
Console::outln("size:"+s.size());
for(var it=s.iter();!it.end();it._next()){
	k,v=it.get();
	Console::outln("k:{0} v:{1} ",k,v);
}