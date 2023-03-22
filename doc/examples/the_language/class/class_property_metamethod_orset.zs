class Number{
	constructor(_value=0){
		// ensure that n is literal integer or literal float
		if(_value instanceof Integer || _value instanceof Float){
			this.__value__=_value;
		}else{
			System::error("Number::constructor : input value not supported");
		}
	} 
	
	// property 'value'
	value{
		_get(){
			return this.__value__
		}
		
		_orset(_op1){
			if(_op1 instanceof Integer || _op1 instanceof Float){
	 			this.__value__ |= Integer::parse(_op1);
	 		}else if(_op1 instanceof Number){
	 			this.__value__ |= Integer::parse(_op1.__value__);
	 		}else{
	 			System::error("Number::value::_orset : right operand not supported");
	 		}
		}
	}
}

var number=new Number(0x1);
Console::outln("number.value => "+number.value)
number.value|=0x2;
Console::outln("number.value|=0x2 => "+number.value)
number.value|=new Number(0x4);
Console::outln("number.value|=new Number(0x4) => "+number.value)