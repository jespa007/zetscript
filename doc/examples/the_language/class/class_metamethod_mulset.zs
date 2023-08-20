class Number{
	constructor(_value=0){
		this.__value__=_value;
	} 
	
 	_mulset(_op1){
 		if(_op1 instanceof Integer || _op1 instanceof Float){
 			this.__value__ *= _op1;
 		}else if(_op1 instanceof Number){
 			this.__value__ *= _op1.__value__;
 		}else{
 			System::error("Number::_mulset : right operand not supported");
 		}
 	}
 	_tostring(){
 		return this.__value__;
 	}
};

var number=new Number(20);
Console::outln("number*=20 => {0}",number*=20)
Console::outln("number*=new Number(30) => {0}",number*=new Number(30))

