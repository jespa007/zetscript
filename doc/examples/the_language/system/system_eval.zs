var operators=[
	"+"
	,"-"
	,"*"
	,"/"
]

for(var operator in operators){
	var operation=String::format("{0} {1} {2}",10,operator,2);
	var result=System::eval(
		"return operation;\n"
	,{
			operation:operation
	});
	
	Console::outln("Result of operation '{0}' => {1} ",operation,result);
}

