function fun(){}

fun();

/*


class  A{
	var fun;
	var a;
	function A(){
		
		var obj=this;
		
		//this=30;
		
		obj.a=20;
		
		print("a:"+this.a);
		
		//this.a=10;
		//print ("g:"+this.a);
		this.fun=(function(obj1){ print ("g:"+obj1.a);});
	}
	
	function print2(){
		this.fun(this);
	}
};


class B:A
{
	//var a;
	
	function B(){
		super();
		//this.a=20;
	}
};*/

/*
function add(
		a
		,b
		,c){
	return a+b+c;
}

function add(a,b){
	return a+b;
}


var add_function_obj=add;

if(add_function_obj(30,40)<add_function_obj(10,0)){
	print("lower");
}else{
	print("higher");
}*/
/*

var d=new B();

d.print2();
*/
