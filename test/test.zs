function f3(a){
	Console::println(a);
}

function f2(a){
	f3(a+3);
}

function f1(a){
	f2(a);
}

var a=0;
f1(a)