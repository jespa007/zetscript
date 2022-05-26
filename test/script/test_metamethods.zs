import "Number.zs"

function test_arithmetic_number_basic(_a,_b){
	var operators=["+","-"]

	for(var op in operators){
		System::eval(
			"System::assert(\n"+
				"(new Number("+_a+")"+op+"new Number("+_b+"))=="+(_a+op+_b)+", \\\"(new Number("+_a+")+new Number("+_b+"))!="+(_a+op+_b)+"\\\"\n"+
			");"
		);
	}
}	

//------------------------------------------------------------------------------------------------
// basic
// test basic arithmetic operations
test_arithmetic_number_basic(1,3)



