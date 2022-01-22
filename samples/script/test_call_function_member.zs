// A class example
class Test{
	
	var a
	Test(a){
		this.a=a; // init built-in
		Console::outln("Test::Test this.a:{0}",this.a);
	}
	function1(a){
		this.a+=10+a;
		Console::outln("Test::function1, this.a:{0}",this.a);
		this.function0();
	}
};

// declaration function member out of its class
function Test::function2(a){
	this.function1(a+5); // it calls Test::function1(a+5)
	this.a+=a;
	Console::outln("Test::function2, this.a:{0}",this.a);
}

// A inheritance class example. TestExtended inherites function1 and function2. 
class TestExtended extends Test{
	
	TestExtended(a){
		Console::outln("TestExtended::TestExtended this.a:{0}",this.a);
		super(a); // it calls base constructor (by default it doesn't call)
		this.function3=function(a){ // creates anonymous function
			this.a+=a; // anonyous functions can access its own 'this' member
			Console::outln("TextExtended::Anonymous function, this.a:{0}",this.a);
			this.function2(a);
		};
	}
	
	function // function keyword is optional in member functions
	function0(a){ 
		this.a+=10; 	
		Console::outln("TestExtended::function0, this.a:{0}",this.a);
	}
	
	function2(a){
		
		super(a+10); // it calls Test::function2(2)
		this.a+=5; 	
		Console::outln("TestExtended::function2, this.a:{0}",this.a);
	}
};

var t=new TestExtended(1);
System::assert(t.a==1,"t.a!=1");
var a=t.function3(20);
System::assert(t.a==111,"t.a!=111");



