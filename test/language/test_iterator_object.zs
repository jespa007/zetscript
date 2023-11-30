var o={a:1,b:"string",c:true}
var k,v;
var rk=""
var rv=""

System::assert(ScriptObject::keys(o).length==3,"ScriptObject::keys(o).length!=3");
	
for(var it=ScriptObject::_iter(o);!it._end();it._next()){
	k,v=it._get();
	rk+=k
	rv+=v
}

System::assert(rk=="abc","rk!=abc")
System::assert(rv=="1stringtrue","rv!=1stringtrue")

