class Test{
	constructor(){
		this.member1=function(){
			Console::outln("call anonymous function member1()")
		}
	}
}

var test=new Test();
test.member1();