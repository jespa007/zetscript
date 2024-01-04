class NumberWithoutString{
	constructor(_value=0){
		this.__value__=_value;
	}
};

class Number{
	constructor(_value=0){
		this.__value__=_value;
	}
	
 	_tostring(){
 		return this.__value__; // returns this.__value__ when string operation is involved
 	}
};

Console::outln("without _tostring => "+new NumberWithoutString(10))
Console::outln("with _tostring => "+new Number(10))