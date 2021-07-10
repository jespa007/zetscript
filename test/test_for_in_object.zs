var obj={
	a:0
	,b:10
	,c:"k"
	,d:true
	,obj:{
		k1:0
		,k2:2
	}
};
class A{
	static _add(n1,n2){}
}

var a=new A();

for(var v in obj){
	 Console::outln("v:{0}",v);
}

for(var k,v in obj 	){
	 Console::outln("k[\"{0}\"]:{1}",k,v);
}

for(var k,v in -obj.obj){
	 Console::outln("k[\"{0}\"]:{1}",k,v);
}
