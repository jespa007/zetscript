// A class example
class Test{
	constructor(){
		print("Test::constructor");
		this.data1 =0; // init data1
	}
	function1(a){
		this.data1+=10+a;
		print("Test::function1 data1:"+this.data1);
	}
	
};

// post-include function member Test::function2
function Test::function2(a){
	this.function1(a+5); // it calls Test::function1(a+5)
	this.data1+=a;
	print("Test::function2 data1:"+this.data1);
}

// A inheritance class example. TestExtended inherites function1 and function2. 
class TestExtended extends Test{
	
	constructor(){
		super(); // it calls base constructor (by default it doesn't call)
		print("TestExtended::constructor");
	}
	
	function2(a){
		super(a+10); 		// it calls Test::function1(2)
		this.data1+=5; 	
		print("TestExtended::function2 data1:"+this.data1);
	}
};

var t=new TestExtended(); 	// instances TestExtended class
t.function2(20); 			// should print be: 