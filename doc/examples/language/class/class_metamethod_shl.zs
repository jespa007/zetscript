class Number{
	constructor(_value=0){
		this.__value__=_value;
	}
	
 	static _shl(_op1,_op2){
 		var op1,op2
 		if(_op1 instanceof Integer || _op1 instanceof Float){
 			op1 = Integer::parse(_op1);// ensure that is integer
 		}else if(_op1 instanceof Number){
 			op1 = Integer::parse(_op1.__value__);// ensure that is integer
 		}else{
 			System::error("Number::_shl : right operand not supported");
 		}
 		
 		if(_op2 instanceof Integer || _op2 instanceof Float){
 			op2 = Integer::parse(_op2);// ensure that is integer
 		}else if(_op2 instanceof Number){
 			op2 =Integer::parse(_op2.__value__);// ensure that is integer
 		}else{
 			System::error("Number::_shl : left operand not supported");
 		}
 		
 		return new Number(op1<<op2); // op1 and op2 are integers and it can perform bitwise shift left with integers
 	}
 	_tostring(){
 		return this.__value__;
 	}
};

Console::outln("new Number(0x1) << new Number(2) => " +  (new Number(0x1) << new Number(2)));
Console::outln("new Number(0x1) << 2 => " + (new Number(0x1) << 2));
Console::outln("0x1 << new Number(2) => " + (0x1 << new Number(2)));
