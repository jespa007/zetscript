
function add(a,b,c){
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

