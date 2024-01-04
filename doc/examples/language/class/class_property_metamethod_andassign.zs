class Number{
	constructor(_value=0){
		this.__value__=_value;
	} 

	// property 'value'
	value{
		_get(){
			return this.__value__
		}
		
		_andassign(_op1){
			if(_op1 instanceof Integer || _op1 instanceof Float){
	 			this.__value__ &= Integer::parse(_op1);
	 		}else{
	 			System::error("Number::value::_andassign : right operand not supported");
	 		}
		}
	}
}

var number=new Number(0xf);
Console::outln("number.value&=0x3 => {0}",number.value&=0x3)
