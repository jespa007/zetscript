var o={a:1,b:"string",c:true}
var k,v;
var rk=""
var rv=""

System::assert(Object::keys(o).size()==3,"Object::keys(o).size()!=3");
	
for(var it=Object::iter(o);!it.end();it++){
	v,k=it.get();
	rk+=k
	rv+=v
	
	Console::outln("k:{0} v:{1} ",k,v);
}

