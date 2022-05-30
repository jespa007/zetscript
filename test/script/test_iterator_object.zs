var o={a:1,b:"string",c:true}
var k,v;
var rk=""
var rv=""

System::assert(Object::keys(o).size()==3,"Object::keys(o).size()!=3");
	
for(var it=Object::iter(o);!it.end();it++){
	k,v=it.get();
	rk+=k
	rv+=v
}

System::assert(rk=="abc","rk!=abc")
System::assert(rv=="1stringtrue","rv!=1stringtrue")

