class Number{
	constructor(_value=0){
		this.__value__=_value;
	} 
	
	_orset(_op1){
 		if(_op1 instanceof Integer || _op1 instanceof Float){
 			this.__value__ |= Integer::parse(_op1);
 		}else if(_op1 instanceof Number){
 			this.__value__ |= Integer::parse(_op1.__value__);
 		}else{
 			System::error("Number::_orset : right operand not supported");
 		}
 	}
 	_tostring(){
 		return this.__value__;
 	}
};

var number=new Number(0x1);
Console::outln("number|=0x2 => {0}",number|=0x2)
Console::outln("number|=new Number(0x4) => {0}",number|=new Number(0x4))

