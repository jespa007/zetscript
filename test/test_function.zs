/*function fun(){}

fun();*/

/*function p(a){
	print("a"+a);
}*/

//var a=function(){ print("hello world");};
//a();

//print("dadas");

function a(s){
	print(s);
}

var b=print;
b("hello world! 1");
b(100);

var v=[
	function(){ return "hello_word";}
	,function(c,d,e,f){ return "hello_word"+b;}
	//,function(c){ return 100+a;}
];

for(var i=0; i < v.length; i++){
	print("v["+i+"]="+v[i](i));
}

/*var v=[
	function(){print("dadas");}
	,function(){print(10);}
];
v[0]();
v[1]();*/
//var a=function(){};
//a();

//IO.clock();

/*function p(f){
	f();
}

p(function(){return 0;},2+3*4);*/

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
