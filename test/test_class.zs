

class Test{
};

var Test::data3;

/*function Test::function3(){
	this.data3="a string";
}*/

class TestExtended: Test{
	
	function function4(){
		this.data3;//this.function2()+this.data1;
	}
};

//var t=new TestExtended();

