class A{

}

class B extends A{

	
}

class C{
	
}

var test_instanceof=[
	"0 instanceof int"
	,"0.0 instanceof float"
	,"true instanceof bool"
	,"[0,1,2] instanceof Array"
	,"\"abc\" instanceof String"
	,"new A() instanceof A"
	,"new B() instanceof A"
	,"new B() instanceof B"
	,"new C() instanceof B"
	,"{a:0,b:1} instanceof ScriptObject"
	
];


for(var v in test_instanceof){
	Console::outln("{0}:{1}",v,System::eval(v))
}


