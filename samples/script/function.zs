function add(a,b,c){
	Console::outln("a:"+a+" b:"+b+" c:"+c+" c==null "+(c==null))
	var d=a+b;
	if(c!=null){
		d+=c;
	}
	return d;
}

var add_function_obj=add;

Console::outln("fun1:"+add_function_obj(30,40)+ " fun2:"+add_function_obj(5,60,5));


