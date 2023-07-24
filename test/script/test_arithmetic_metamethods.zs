import "Number.zs"

function test_arithmetic(_a,_b){
	var operators=["+","-","*","/","%","^","&","<<",">>"]

	// normal
	for(var i=0; i < 2; i++){
		for(var op in operators){
			System::eval(
				"System::assert(\n"+
					"(new Number("+_a+")"+op+"new Number("+_b+"))==Float::parse("+(_a+op+_b)+"), \\\"(new Number("+_a+") "+op+" new Number("+_b+"))!=\\\"+("+(_a+op+_b)+")\n"+
				");"
			);
		}
		
		// reverse
		_a,_b=_b,_a
	}
}

function test_arithmetic_set(_a,_b){
	var operators=["+=","-=","*=","/=","%=","^=","&=","<<=",">>="]

	// normal
	for(var i=0; i < 2; i++){
		var a=_a;
		var b=_b;
		for(var op in operators){
			System::eval(
				"var ma=new Number(a),mb=new Number(b);\n" +
				"var va=a,vb=b;\n" +
				" ma"+op+"mb\n;"+
				" va"+op+"vb\n;"+
				"System::assert(\n"+
					"ma==va, \\\"test_arithmetic_set ma("+a+op+_b+" => \\\"+ma.value+\\\")!=\\\"+va\n"+
				 ");"					

				,{
					a:a
					,b:b
				}
			);
		}
		
		// reverse
		_a,_b=_b,_a
	}
}

function test_arithmetic_set_property(_a,_b){
	var operators=["+=","-=","*=","/=","%=","^=","&=","<<=",">>="]

	// normal
	for(var i=0; i < 2; i++){
		var a=_a;
		var b=_b;
		for(var op in operators){
			System::eval(
				"var ma=new Number(a),mb=new Number(b);\n" +
				"var va=a,vb=b;\n" +
				" ma.value"+op+"mb.value\n;"+
				" va"+op+"vb\n;"+
				"System::assert(\n"+
					"ma==va, \\\"test_arithmetic_set_property ma("+a+op+_b+" => \\\"+ma.value+\\\")!=\\\"+va\n"+
				 ");"					
				,{
					a:a
					,b:b
				}
			);
		}
		
		// reverse
		_a,_b=_b,_a
	}
}

function test_self_operation(_a){
	
	var operations=["{0}++","{0}--","++{0}","--{0}"]
	
	for(var op in operations){
		System::eval(
			"var ma=new Number("+_a+");\n" +
			"var va="+_a+";\n" +
			String::format(op,"ma")+";\n"+
			String::format(op,"va")+";\n"+
			"System::assert(\n"+
				"ma.value==va, \\\"test_self_operation "+String::format(op,"ma")+" => \\\"+ma.value+\\\")!=\\\"+va\n"+
			 ");"					
		);
	}
}

function test_self_operation_property(_a){
	var operations=["-{0}","{0}++","{0}--","++{0}","--{0}","-{0}++","-{0}--"]
	
	for(var op in operations){
		System::eval(
			"var ma=new Number("+_a+");\n" +
			"var va="+_a+";\n" +
			"/*System::assert(\n"+
				String::format(op,"ma.value")+"=="+String::format(op,"va")+",\\\"test_self_operation_property "+String::format(op,"ma")+" => \\\"+ma.value+\\\"!=\\\"+va\n"+
			 ");*/"					
		);
	}	
}


//------------------------------------------------------------------------------------------------
// basic
// integer arithemtic operations
test_arithmetic(20,10)

// check arithemtic metamethods
test_arithmetic_set(20,10)

// check arithemtic properties
test_arithmetic_set_property(20,10)

// check self operations
test_self_operation(10)

// check properties self operations
test_self_operation_property(10)



