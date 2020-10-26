// A class example
class Test{
	constructor(){
		print("Test::constructor");
		this.a =0; // init a
	}
	function1(a){
		this.a+=10+a;
		print("Test::function1, this.a:"+this.a);
		this.function0();
	}
};

// post-include function member Test::function2
function Test::function2(a){
	this.function1(a+5); // it calls Test::function1(a+5)
	this.a+=a;
	print("Test::function2, this.a:"+this.a);
}

// A inheritance class example. TestExtended inherites function1 and function2. 
class TestExtended extends Test{
	
	constructor(){
		print("TestExtended::constructor");
		super(); // it calls base constructor (by default it doesn't call)
		this.function3=function(a){ // creates anonymous function with this context
			this.a+=a;
			print("TextExtended::Anonymous function, this.a:"+this.a);
			this.function2(a);
		};
	}
	
	function0(a){
		this.a+=10; 	
		print("TestExtended::function0, this.a:"+this.a);
	}
	
	function2(a){
		super(a+10); 		// it calls Test::function1(2)
		this.a+=5; 	
		print("TestExtended::function2, this.a:"+this.a);

	}
};

var t=new TestExtended(); 	// instances TestExtended class
t.function3(20);
