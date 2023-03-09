class Number{
	constructor(_value=0){
		// ensure that n is literal integer or literal float
		if(_value instanceof Integer || _value instanceof Float){
			this.__num__=_value;
		}else{
			System::error("Number::constructor : input value not supported");
		}
	}
	
 	static _shr(_op1,_op2){
 		var op1,op2
 		if(_op1 instanceof Integer || _op1 instanceof Float){
 			op1 = Integer::parse(_op1); // ensure that is integer
 		}else if(_op1 instanceof Number){
 			op1 = Integer::parse(_op1.__num__); // ensure that is integer
 		}else{
 			System::error("Number::_shr : right operand not supported");
 		}
 		
 		if(_op2 instanceof Integer || _op2 instanceof Float){
 			op2 = Integer::parse(_op2); // ensure that is integer
 		}else if(_op2 instanceof Number){
 			op2 = Integer::parse(_op2.__num__) // ensure that is integer;
 		}else{
 			System::error("Number::_shr : left operand not supported");
 		}
 		
 		return new Number(op1>>op2); // op1 and op2 are integers and it can perform bitwise shift right with integers
 	}
 	_tostring(){
 		return this.__num__;
 	}
};

Console::outln("new Number(10) >> new Number(20) => " +  (new Number(10) >> new Number(20)));
Console::outln("new Number(10) >> 20 => " + (new Number(10) >> 20));
Console::outln("10 >> new Number(20) => " + (10 >> new Number(20)));
