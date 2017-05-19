class A{
	var i;
	
	function A(j){
		var f=j;
		
		print("hola"+f);
		this.i=f;
		
		f=2;
		
		print("hola"+this.i++);
		
		print("hola"+this.i++);
	}
};

var k="hola caca";

var a = new A(k);

//print(""+a.i);