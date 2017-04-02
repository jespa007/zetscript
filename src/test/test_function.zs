function add(a,b){
	return a+b;
}

var add_function=add;

add(0,1)==add_function(0,1);

//((function(a,b){return a+b;}(0,1)) == 1) 
