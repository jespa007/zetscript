class Number{
	constructor(_value=0){
		// ensure that n is literal integer or literal float
		if(_value instanceof Integer || _value instanceof Float){
			this.__value__=_value;
		}else{
			System::error("Number::constructor : input value not supported");
		}
	} 
	
 	_xorset(_op1){
 		if(_op1 instanceof Integer || _op1 instanceof Float){
 			this.__value__ ^= Integer::parse(_op1);
 		}else if(_op1 instanceof Number){
 			this.__value__ ^= Integer::parse(_op1.__value__);
 		}else{
 			System::error("Number::_xorset : right operand not supported");
 		}
 	}
 	_tostring(){
 		return this.__value__;
 	}
};

var number=new Number(0);
Console::outln("number => "+number)
number^=0xa;
Console::outln("number^=0xa => "+number)
number^=new Number(0x9);
Console::outln("number^=new Number(0x9) => "+number)

