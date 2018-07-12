var g=0;

class  A{
	var fun;
	//var a;
	function A(){
		//this.a=10;
		//print ("g:"+this.a);
		this.fun=function(){ print ("g:");};
	}
	
	function print(){
		this.fun();
	}
};

class B:A
{
	var a;
	
	function B(){
		//this.a=20;
	}
};


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
}

var d=new B();

d.print();

