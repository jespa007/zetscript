import "Number.zs"

function test_arithmetic_number_basic(_a,_b){
	/*var operators=["+","-"]

	for(var op in operators){
		Console::outln(
			"((new Number("+_a+"))"+op+"(new Number("+_b+")))==("+(_a+op+_b)+"), \\\"(new Number("+_a+"))"+op+"(new Number("+_b+"))!=\\\"+("+(_a+op+_b)+")\n"
		);
		System::eval(
			//"System::assert(0==0,\\\"caca\\\")"
			"System::assert(\n"
				"((new Number("+_a+"))+(new Number("+_b+")))==("+(_a+op+_b)+"), \\\"(new Number("+_a+"))+(new Number("+_b+"))!=\\\"("+(_a+op+_b)+")\n"
			");"
		);
	}*/
}	

//------------------------------------------------------------------------------------------------
// basic
// test basic arithmetic operations
//System::assert(((new Number(1))+(new Number(3)))==4), "(new Number(1))+(new Number(3))!=4)"
//System::assert(((new Number(1))-(new Number(3)))==-2),"(new Number(1))-(new Number(3))!=-2)"
test_arithmetic_number_basic(1,3)

((new Number(1))+(new Number(3)));//==(1+3)//, "(new Number(1))+(new Number(3))!="+(1+3)
//((new Number(1))-(new Number(3)))==-2),"(new Number(1))-(new Number(3))!=-2
//((new Number(1))+(new Number(3)))==(1+3), "(new Number(1))+(new Number(3))!="+(1+3)