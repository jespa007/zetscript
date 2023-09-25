class Number{
	constructor(_value=0){
		this.__value__=_value;
	} 
	
	// property 'value'
	value{
		_get(){
			return this.__value__
		}
		
		_mulst(_op1){
			if(_op1 instanceof Integer || _op1 instanceof Float){
	 			this.__value__ *= _op1;
	 		}else{
	 			System::error("Number::value::_mulst : right operand not supported");
	 		}
		}
	}
}

var number=new Number(10);
Console::outln("number.value*=10 => {0}",number.value*=10)
