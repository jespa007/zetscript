

class Test{
	var data1;
	var data2;
	
	function function1(a){
		this.data1 =a;
		print("calling from Test. data1:"+this.data1);
	}
	
	function function2(){
		this.data2=3.0;
		return this.data2;
	}
};

var Test::data3;

function Test::function3(){
	this.data3="a string";
}

class TestExtended: Test{
	var data4;
	function function1(a){
		super(2); // it calls Test::function1(2)
		this.data1+=5; // Now data1=5+2 = 7
		print("calling from TestExtended. Data1:"+this.data1);
	}
	
	function function4(){
		this.data4=6;
		this.data4=this.data3+this.function2()+this.data1;
	}
};




var t=new TestExtended();
var i=t.function1(2); // initializes data2 as 3.0 and return the value

