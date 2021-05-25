class A{
	_in(k){
		if(k<10 || k > 20){
			return false;
		}
		return true;
	}
	
}

var test_instanceof=[
	"0 instanceof int"
	,"0.0 instanceof float"
	,"true instanceof bool"
	,"[0,1,2] instanceof ScriptObjectVector"
	,"{'a':0,'b':1} instanceof ScriptObjectObject"
];


for(var v in test_instanceof){
	Console::outln("{0}:{1}",v,System::eval(v))
}


