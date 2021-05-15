import "A.zs"

class B extends A{
	function function1(){
		super();
		Console::outln("call from B");
	}
};