function add(a,b,c){
	print("a:"+a+" b:"+b+" c:"+c+" c==undefined"+(c==undefined))
	var d=a+b;
	if(c!=undefined){
		d+=c;
	}
	return d;
}

var add_function_obj=add;

print("fun1:"+add_function_obj(30,40)+ " fun2:"+add_function_obj(5,60,5));


