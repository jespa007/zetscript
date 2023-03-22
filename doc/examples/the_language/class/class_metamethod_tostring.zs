class NumberWithoutString{
	constructor(_value=0){
		// ensure that n is literal integer or literal float
		if(_value instanceof Integer || _value instanceof Float){
			this.__value__=_value;
		}else{
			System::error("Number::constructor : input value not supported");
		}
	}
};

class Number{
	constructor(_value=0){
		// ensure that n is literal integer or literal float
		if(_value instanceof Integer || _value instanceof Float){
			this.__value__=_value;
		}else{
			System::error("Number::constructor : input value not supported");
		}
	}
	
 	_tostring(){
 		return this.__value__; // returns this.__value__ when string operation is involved
 	}
};

Console::outln("without _tostring => "+new NumberWithoutString(10))
Console::outln("with _tostring => "+new Number(10))