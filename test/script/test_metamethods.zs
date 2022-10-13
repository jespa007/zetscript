import "Integer.zs"
import "Number.zs"

function test_arithmetic_integer_basic(_a,_b){
	var operators=["+","-","*","/","%","^","&","<<",">>"]

	// normal
	for(var i=0; i < 2; i++){
		for(var op in operators){
			System::eval(
				"System::assert(\n"+
					"(new Integer("+_a+")"+op+"new Integer("+_b+"))==parseInteger("+(_a+op+_b)+"), \\\"(new Integer("+_a+") "+op+" new Integer("+_b+"))!=\\\"+parseInteger("+(_a+op+_b)+")\n"+
				");"
			);
		}
		
		// reverse
		_a,_b=_b,_a
	}
}


function test_arithmetic_number_basic(_a,_b){
	var operators=["+","-","*","/","%"]

	// normal
	for(var i=0; i < 2; i++){
		for(var op in operators){
			System::eval(
				"System::assert(\n"+
					"(new Number("+_a+")"+op+"new Number("+_b+"))=="+(_a+op+_b)+", \\\"(new Number("+_a+")"+op+"new Number("+_b+"))!=\\\"+("+(_a+op+_b)+")\n"+
				");"
			);
		}
		// reverse
		_a,_b=_b,_a
	}
		
}	


function test_arithmetic_integer_set(_a,_b){
	var operators=["+=","-=","*=","/=","%=","^=","&=","<<=",">>="]

	// normal
	for(var i=0; i < 2; i++){
		var a=_a;
		var b=_b;
		for(var op in operators){
			System::eval(
				"var ma=new Integer(a),mb=new Integer(b);\n" +
				"var va=a,vb=b;\n" +
				" ma"+op+"mb\n;"+
				" va"+op+"vb\n;"+
				"System::assert(\n"+
					"ma==va, \\\"ma("+a+op+_b+")!=\\\"+va\n"+
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

//------------------------------------------------------------------------------------------------
// basic
// integer arithmetic operations
test_arithmetic_integer_basic(20,10)
test_arithmetic_number_basic(96,5)

// check aritemthic metamethods
test_arithmetic_integer_set(20,10)
//test_arithmetic_number_set(96,5)

// check aritemthic properties
//test_arithmetic_integer_set(20,10)
//test_arithmetic_number_set(96,5)


