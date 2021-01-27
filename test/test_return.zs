class A{
	var i;
	function A(_i){
		this.i=_i;		
	}
	function return_this(){
		return this;
	}
};


function ret_vect(){
	return new A(10); 
}

function ret_expr(){
	return 5+3;
}

function ret_multiple(i){
	return 1+i,2+i,3+i;
}

function ret_value(){
	return -10;
}

function ret_struct(){
	return {a:10,b:"10"};
}

function ret_variable(){
	var b=new A(10);
	
	return b;
}

var n1,n2,n3;
n1,n2,n3=ret_multiple(10);
var a=ret_vect();
var e=ret_expr();
var v=ret_value();
var c=ret_variable();
var s=ret_struct();

print("a.i:"+a.i);
print("e:"+e);
print("s:"+s.b);
print("v:"+v);
print("c.i:"+c.i);
print("ret_variable().i:"+ret_variable().i);