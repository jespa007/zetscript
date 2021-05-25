// A class example
class Test{
	const MAX_NUM=9
	
	// built-in member vars with optional initialization
	var a,b=-1,c=-2

	Test(a,b=-1,c=Test::MAX_NUM){
		this.a=a; // init built-in
		this.b=b; // init built-in
		this.c=c;  // init built-in
		this.d=-100; // a user property
		this["e"]=-200; // another user property usign []
		Console::outln("Test::constructor this.a:{0} this.b:{1} this.c:{2} this.d:{3} this.d:{4} MAX_NUM:{5}"
				,this.a
				,this.b
				,this.c
				,this.d
				,this.e
				,Test::MAX_NUM);
		
	}
	function1(a){
		this.a+=10+a;
		Console::outln("Test::function1, this.a:{0}",this.a);
		this.function0();
	}
};

// post-include function member
function Test::function2(a){
	this.function1(a+5); // it calls Test::function1(a+5)
	this.a+=a;
	Console::outln("Test::function2, this.a:{0}",this.a);
}

// late bind var and const member
var Test::f=-1,Test::e
const Test::MAX_NUM_EXT=10

// A inheritance class example. TestExtended inherites function1 and function2. 
class TestExtended extends Test{
	
	TestExtended(a){
		Console::outln("TestExtended::constructor this.a:{0}",this.a);
		super(a); // it calls base constructor (by default it doesn't call)
		this.function3=function(a){ // creates anonymous function and take account this context
			this.a+=a;
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

var t=new TestExtended(1); 	// instances TestExtended class
t.function3(20); // it calls anynomus function*/
