class A{
	var i;
	function A(_i){
		this.i=_i;		
	}
	function return_this(){
		return this;
	}
};

// test all almost return cases
var test_ret=[
	 function(){return -10;}
	,function(){var b=-10;return b;}
	,function(){return 1,2,3;}
	,function(){var a,b,c; a,b,c=1,2,3;return a,b,c}
	,function(){return "abc";}
	,function(){var b="abc";return b;}
	,function(){return [5,3];}
	,function(){var b=[5,3];return b;}
	,function(){return 5+3;}
	,function(){var b=5+3;return b;}
	,function(){return {a:10,b:"10"};}
	,function(){var b={a:10,b:"10"}; return b;}
	,function(){return new A(10);}
	,function(){var b=new A(10);return b;}
	,function(){return (new A(10)).return_this();}
	,function(){var b=new A(10);return b.return_this();}
];

function return_test_from_function(f){
	return f()
}

Console::outln("return test:")

for(var f in test_ret){
	Console::outln(f())
}

Console::outln("return test from function:")


for(var f in test_ret){
	Console::outln(return_test_from_function(f))
}