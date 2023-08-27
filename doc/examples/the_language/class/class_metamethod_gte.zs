class Number{
	constructor(_value=0){
		this.__value__=_value;
	} 
	
 	static _gte(_op1, _op2){
 		var op1,op2
 		if(_op1 instanceof Integer || _op1 instanceof Float){
 			op1 = _op1;
 		}else if(_op1 instanceof Number){
 			op1 = _op1.__value__;
 		}else{
 			System::error("Number::_gte : right operand not supported");
 		}
 		
 		if(_op2 instanceof Integer || _op2 instanceof Float){
 			op2 = _op2;
 		}else if(_op2 instanceof Number){
 			op2 = _op2.__value__;
 		}else{
 			System::error("Number::_gte : left operand not supported");
 		}	
 		return op1>=op2;
 	}
};

Console::outln("new Number(20) >= 20 => "+(new Number(20) >= 20))
Console::outln("20 >= new Number(30) => "+(20 >= new Number(30)))
Console::outln("new Number(30) >= new Number(20) => "+(new Number(30) >= new Number(20)))
