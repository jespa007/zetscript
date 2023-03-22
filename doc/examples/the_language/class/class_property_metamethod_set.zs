class Number{
	constructor(_value=0){
		this.value=_value // calls Number::value::_set metamethod
	} 
	
	// property 'value'
	value{
		_get(){
			return this.__value__
		}
		
		_set(_value){
	 		if(_value instanceof Integer || _value instanceof Float){
	 			this.__value__ = _value;
	 		}else if(_value instanceof Number){
	 			this.__value__ = _value.__value__;
	 		}else{
	 			System::error("Number::value::_set : parameter not supported");
	 		}
		}
	}
}

var number=new Number(20);
Console::outln("On operation 'number=new Number(20)' then 'number.value' is => "+number.value)

number.value = 10;
Console::outln("On operation 'number=10' the 'number.value' is => "+number.value)
