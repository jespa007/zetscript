class B{};

class A{
	constructor(){
		//this=0;
		this.a=this; //<-- this_variable
		//this.b=1; //<-- this_variable
		
		//print("c:"+this.c.a);
		//this.c=0;
		//this.b="hola";
		//this.c=this;
		//this=0;
		//this.c.b=0;
		/*this.a=function(c){
			return this.b+c;			
		};*/
	}
	
	f(){
		return this;
	}

	/*toString(){
		return "mierda";
	}*/
};
/*
function f(a){
	return a;
}*/
/*

var a=undefined;

function f(){
	var g=new A();
	a=g;
	//a=0;
}*/
var a=new A();
a.f();
print("a:"+a.a);
//print("a:"+a);

//f();

//print("a:"+a.a);

//a=a.c;
//a.toString();

/*var d=f(
		g
		.a(10)
);*/

//print(d);