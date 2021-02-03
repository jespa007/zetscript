// A class example
class Test{
	const MAX_NUM=9
	
	// pre create vars with initialization
	var b=-1,c=-2

	Test(a,b=-1,c=Test::MAX_NUM){
		this.a=a; // init a (dynamic)
		this.b=b
		this.c=c
		System::println("Test::constructor this.a:{0} this.b:{1} this.c:{2} this.d:{3} this.e:{4} this.f:{4} MAX_NUM:{2}"
				,this.a
				,this.b
				,this.c
				,this.d
				,this.e
				,this.f
				,Test::MAX_NUM);
		
	}
	function1(a){
		this.a+=10+a;
		System::println("Test::function1, this.a:{0}",this.a);
		this.function0();
	}
};

// post-include function member
function Test::function2(a){
	this.function1(a+5); // it calls Test::function1(a+5)
	this.a+=a;
	System::println("Test::function2, this.a:{0}",this.a);
}

// post-include var and const member
var Test::f=-1,Test::e
const Test::MAX_NUM_EXT=10

// A inheritance class example. TestExtended inherites function1 and function2. 
class TestExtended extends Test{
	
	TestExtended(a){
		System::println("TestExtended::constructor this.a:{0}",this.a);
		super(a); // it calls base constructor (by default it doesn't call)
		this.function3=function(a){ // creates anonymous function with this context
			this.a+=a;
			System::println("TextExtended::Anonymous function, this.a:{0}",this.a);
			this.function2(a);
		};
	}
	
	function0(a){
		this.a+=10; 	
		System::println("TestExtended::function0, this.a:{0}",this.a);
	}
	
	function2(a){
		super(a+10); 		// it calls Test::function1(2)
		this.a+=5; 	
		System::println("TestExtended::function2, this.a:{0}",this.a);

	}
};

var t=new TestExtended(1); 	// instances TestExtended class
t.function3(20); // it calls anynomus function
