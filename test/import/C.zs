import "B.zs"

class C extends B{
	function function1(){
		super();
		Console::outln("call from C");
	}
};