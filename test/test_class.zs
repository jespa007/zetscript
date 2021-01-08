// A class example
class Test{
	const MAX_NUM=9
	
	// setup
	var b=-1,c=-2 //--> like doing this.b=-1 in constructor

	Test(){
		this.a =0; // init a
		System::println("Test::constructor a:{0} b:{1} c:{2} MAX_NUM:{2}",this.a,this.b,this.c,Test::MAX_NUM);
		
	}
	function1(a){
		this.a+=10+a;
		System::println("Test::function1, this.a:{0}",this.a);
		this.function0();
	}
};

// post-include function member Test::function2
function Test::function2(a){
	this.function1(a+5); // it calls Test::function1(a+5)
	this.a+=a;
	System::println("Test::function2, this.a:{0}",this.a);
}

// A inheritance class example. TestExtended inherites function1 and function2. 
class TestExtended extends Test{
	
	TestExtended(){
		System::println("TestExtended::constructor");
		super(); // it calls base constructor (by default it doesn't call)
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

var t=new TestExtended(); 	// instances TestExtended class
t.function3(20);
