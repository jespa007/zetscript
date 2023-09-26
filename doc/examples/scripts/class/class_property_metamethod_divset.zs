class Number{
	constructor(_value=0){
		this.__value__=_value;
	} 
	
	// property 'value'
	value{
		_get(){
			return this.__value__
		}
		
		_divassign(_op1){
			if(_op1 instanceof Integer || _op1 instanceof Float){
	 			this.__value__ /= _op1;
	 		}else{
	 			System::error("Number::value::_divassign : right operand not supported");
	 		}
		}
	}
}

var number=new Number(10);
Console::outln("number.value/=20 => {0}",number.value/=20)
