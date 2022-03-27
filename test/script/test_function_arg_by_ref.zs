class A{
	_toString(){
		return "custom string";
	}
};



var functions=[
	function(a,b,c){
		// overwrite argument values
		Console::outln("function passing args by copy");
		a=0;
		b=0;
		c=0;
		Console::outln("in function post a:"+a+" b:"+b+" c:"+c);
	},function(ref a,ref b, ref c){
		// overwrite value refs
		Console::outln("function passing args by ref");
		a=0;
		b=0;
		c=0;
		Console::outln("in function post a:"+a+" b:"+b+" c:"+c);
	}
];


for(var i=0; i < functions.size(); i++)
{
	var a=10,b=10,c=new A();
	
	Console::outln("----");
	Console::outln("before call a:"+a+" b:"+b+" c:"+c);
	
	functions[i](a,b,c);
	
	Console::outln("after call a:"+a+" b:"+b+" c:"+c);
}



