class A{
	var i;
	function A(_i){
		this.i=_i;		
	}
};


function ret_vect(){
	return new A(10); 
}

function ret_expr(){
	return 5+3;
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

var a=ret_vect();
var e=ret_expr();
var v=ret_value();
var c=ret_variable();
var s=ret_struct();

print("a.i:"+a.i);
print("e:"+e);
print("s:"+s.b);
print("v:"+v);
print("c:"+c.i);