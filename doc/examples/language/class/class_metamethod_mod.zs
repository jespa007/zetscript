class Number{
	constructor(_value=0){
		this.__value__=_value;
	} 
	
 	static _mod(_op1,_op2){
 		var op1,op2
 		if(_op1 instanceof Integer || _op1 instanceof Float){
 			op1 = _op1;
 		}else if(_op1 instanceof Number){
 			op1 = _op1.__value__;
 		}else{
 			System::error("Number::_mod : right operand not supported");
 		}
 		
 		if(_op2 instanceof Integer || _op2 instanceof Float){
 			op2 = _op2;
 		}else if(_op2 instanceof Number){
 			op2 = _op2.__value__;
 		}else{
 			System::error("Number::_mod : left operand not supported");
 		}
 		
 		return new Number(op1%op2);
 	}
 	_tostring(){
 		return this.__value__;
 	}
};

Console::outln("new Number(10) % new Number(20) => " +  (new Number(10) % new Number(20)));
Console::outln("new Number(10) % 20 => " + (new Number(10) % 20));
Console::outln("10 % new Number(20) => " + (10 % new Number(20)));
