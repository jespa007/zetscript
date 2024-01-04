function generate_function(){
	var i=0;
	return function(){
		Console::outln("i:"+i)
		i++;
	}() 
	
}

function(){
	Console::outln("i1")
	//i++;
}()

var f=generate_function();

//f();