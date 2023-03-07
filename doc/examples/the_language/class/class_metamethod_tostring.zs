class NumberWithoutString{
	constructor(_n){
 		this.__num__=_n;
	}
};

class Number{
	constructor(_n){
 		this.__num__=_n;
	}
 	_tostring(){
 		return this.__num__;
 	}
};

Console::outln("without _tostring => "+new NumberWithoutString(10))
Console::outln("with _tostring => "+new Number(10))