class Number{
	constructor(_value=0){
		// ensure that n is literal integer or literal float
		if(_value instanceof Integer || _value instanceof Float){
			this.__num__=_value;
		}else{
			System::error("Number::constructor : input value not supported");
		}
	} 
	
 	_addset(_op1){
 		if(_op1 instanceof Integer || _op1 instanceof Float){
 			this.__num__ += _op1;
 		}else if(_op1 instanceof Number){
 			this.__num__ += _op1.__num__;
 		}else{
 			System::error("Number::_addset : right operand not supported");
 		}
 	}
 	_tostring(){
 		return this.__num__;
 	}
};

var number=new Number(20);
Console::outln("number => "+number)
number+=20;
Console::outln("number+=20 => "+number)
number+=new Number(30);
Console::outln("number+=new Number(30) => "+number)

