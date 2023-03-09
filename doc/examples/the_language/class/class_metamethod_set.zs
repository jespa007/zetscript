class Number{
	constructor(_value=0){
		// ensure that n is literal integer or literal float
		if(_value instanceof Integer || _value instanceof Float){
			this.__num__=_value;
		}else{
			System::error("Number::constructor : input value not supported");
		}
	} 
 	_set(_value){
 		if(_value instanceof Integer || _value instanceof Float){
 			this.__num__ = _value;
 		}else if(_value instanceof Number){
 			this.__num__ = _value.__num__;
 		}else{
 			System::error("Number::set : parameter not supported");
 		}
 	}
 	_tostring(){
 		return this.__num__;
 	}
};

var n=new Number ();
Console::outln("n => "+n)

n=10;
Console::outln("n=10 => "+n)

n=new Number(20);
Console::outln("n=new Number(20) => "+n)

