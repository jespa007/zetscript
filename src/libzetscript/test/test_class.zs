

class MyClass{
	var k;
	
	function MyClass(a,b){
		this.k=0;
		print("CONSTRUCTOR 1"+this.k);
	}
	
	function kk(){
		print("Hello MyClass"+this.k);
	}
	
};
/*
class MyClass2:MyObject{
	
	
	function MyClass2(){
		//super();
		//print("CONSTRUCTOR 2");
	}
	
	function print(){
		print("Hello MyClass 2");
		super(); 
	}
	
	function prova(g){
		super(g);
	}
	
};*/


var yy=1;

var g=new MyClass(yy,1);
g.kk();

//print(""+a.i);