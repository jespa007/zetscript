import "Integer.zs"
import "Number.zs"

function test_arithmetic_basic(_a,_b){
	var operators=["+","-","*","/","%","^","&","<<",">>"]

	// normal
	for(var i=0; i < 2; i++){
		for(var op in operators){
			System::eval(
				"System::assert(\n"+
					"(new Number("+_a+")"+op+"new Number("+_b+"))==parseFloat("+(_a+op+_b)+"), \\\"(new Number("+_a+") "+op+" new Number("+_b+"))!=\\\"+("+(_a+op+_b)+")\n"+
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
					"ma==va, \\\"ma("+a+op+_b+" => \\\"+ma.num+\\\")!=\\\"+va\n"+
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

function test_arithmetic_property(_a,_b){
	var operators=["+=","-=","*=","/=","%=","^=","&=","<<=",">>="]

	// normal
	for(var i=0; i < 2; i++){
		var a=_a;
		var b=_b;
		for(var op in operators){
			System::eval(
				"var ma=new Number(a),mb=new Number(b);\n" +
				"var va=a,vb=b;\n" +
				" ma.num"+op+"mb.num\n;"+
				" va"+op+"vb\n;"+
				"System::assert(\n"+
					"ma==va, \\\"ma("+a+op+_b+" => \\\"+ma.num+\\\")!=\\\"+va\n"+
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
test_arithmetic_basic(20,10)

// check aritemthic metamethods
test_arithmetic_set(20,10)

// check aritemthic properties
test_arithmetic_property(20,10)



