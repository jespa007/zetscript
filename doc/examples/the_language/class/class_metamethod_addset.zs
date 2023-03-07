class Number{
	constructor(_n){
 		this.__num__=_n;
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

var sum=new Number(20);
Console::outln("sum => "+sum)
sum+=20;
Console::outln("sum+=20 => "+sum)
sum+=new Number(30);
Console::outln("sum+=new Number(30) => "+sum)

