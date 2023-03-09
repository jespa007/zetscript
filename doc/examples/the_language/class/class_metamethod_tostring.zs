class NumberWithoutString{
	constructor(_value=0){
		// ensure that n is literal integer or literal float
		if(_value instanceof Integer || _value instanceof Float){
			this.__num__=_value;
		}else{
			System::error("Number::constructor : input value not supported");
		}
	}
};

class Number{
	constructor(_value=0){
		// ensure that n is literal integer or literal float
		if(_value instanceof Integer || _value instanceof Float){
			this.__num__=_value;
		}else{
			System::error("Number::constructor : input value not supported");
		}
	}
	
 	_tostring(){
 		return this.__num__;
 	}
};

Console::outln("without _tostring => "+new NumberWithoutString(10))
Console::outln("with _tostring => "+new Number(10))