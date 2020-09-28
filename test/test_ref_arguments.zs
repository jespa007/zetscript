/*class A{
	_toString(){
		return "custom string";
	}
};

*/
var functions=[
	function(a,b,c){
		// overwrite argument values
		print("function passing args by copy");
		a=0;
		b=0;
		c=0;
		print("in function post a:"+a+" b:"+b+" c:"+c);
	},function(ref a,ref b, ref c){
		// overwrite value refs
		print("function passing args by ref");
		a=0;
		b=0;
		c=0;
		print("in function post a:"+a+" b:"+b+" c:"+c);
	}
];


//var functions=[];

for(var i=0; i < 10; i++){
	var a=10; // <-- it crashes local vars?!?!
	/*var a=10,b=10;//,c=new A();
	
	print("before call a:"+a+" b:"+b+" c:"+c);
	
	
	functions[i](a,b,c);
	
	print("after call a:"+a+" b:"+b+" c:"+c);*/
}

