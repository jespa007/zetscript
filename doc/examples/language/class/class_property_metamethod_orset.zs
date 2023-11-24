class Number{
	constructor(_value=0){
		this.__value__=_value;
	} 
	
	// property 'value'
	value{
		_get(){
			return this.__value__
		}
		
		_orassign(_op1){
			if(_op1 instanceof Integer || _op1 instanceof Float){
	 			this.__value__ |= Integer::parse(_op1);
	 		}else{
	 			System::error("Number::value::_orassign : right operand not supported");
	 		}
		}
	}
}

var number=new Number(0x1);
Console::outln("number.value|=0x2 => {0}",number.value|=0x2)
