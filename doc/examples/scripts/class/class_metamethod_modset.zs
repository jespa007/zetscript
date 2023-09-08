class Number{
	constructor(_value=0){
		this.__value__=_value;
	} 
	
 	_modset(_op1){
 		if(_op1 instanceof Integer || _op1 instanceof Float){
 			this.__value__ %= _op1;
 		}else if(_op1 instanceof Number){
 			this.__value__ %= _op1.__value__;
 		}else{
 			System::error("Number::_modset : right operand not supported");
 		}
 	}
 	_tostring(){
 		return this.__value__;
 	}
};

var number=new Number(250);
Console::outln("number%=30 => {0}",number%=30)
Console::outln("number%=new Number(100) => {0}",number%=new Number(100))

