class Number{
	constructor(_value=0){
		// ensure that n is literal integer or literal float
		if(_value instanceof Integer || _value instanceof Float){
			this.__num__=_value;
		}else{
			System::error("Number::constructor : input value not supported");
		}
	} 
	
 	static _equ(op1, op2){
 		return op1.__num__==op2.__num__;
 	}
};

Console::outln("new Number(20) == new Number(20) => "+(new Number(20) == new Number(20)))
Console::outln("new Number(20) == new Number(30) => "+(new Number(20) == new Number(30)))