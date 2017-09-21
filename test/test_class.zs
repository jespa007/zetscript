

class Test{
	var data1;
	var data2;
	
	function function1(a){
		this.data1 =a;
		print("data1:"+this.data1);
	}
	
	function function2(){
		this.data2=3.0;
		return this.data2;
	}
};

var Test::value3;

function Test::function3(){
	this.value3="a string";
}

class TestExtended: Test{
	var data4;
	function function1(){
		super(2); // it calls Test::function1(2)
		this.value1+=5; // Now value1=5+2 = 7
		print(“value1:”+this.value1);
	}
	
	function function4(){
		this.data4=6;
		this.value4=This.value3+this.function2()+this.value1;
	}
};




var t=new Test();

t.function1(2); // initializes data1 as 2
var i=t.function1(2); // initializes data2 as 3.0 and return the value
print("data1 is:"+t.data1); // prints value of data1


//print(""+a.i);