class Test{
	
	constructor(){
		Console::outln("call from constructor()")
		this.member1();
	}
	
	member1(){
		Console::outln("call from member1()")
	}
	
	member2(){
		Console::outln("call from member2()")
	}
}

var test=new Test();

test.member2();