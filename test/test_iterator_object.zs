var o={a:1,b:2,o:{
		k1:0
		,k2:2
	}}
var k,v;
Console::outln("size:"+Object::keys(o).size());
for(var it=Object::iter(o);!it.end();it++){
	k,v=it.get();
	Console::outln("k:{0} v:{1} ",k,v);
}