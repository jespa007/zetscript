class Number{
	constructor(_value=0){
		// ensure that n is literal integer or literal float
		if(_value instanceof Integer || _value instanceof Float){
			this.__value__=_value;
		}else{
			System::error("Number::constructor : input value not supported");
		}
	} 

	value{
		_get(){
			return this.__value__
		}
		
		_predec(){
			this.__value__-=1;
		}
	}
}

var number=new Number(20);
Console::outln("--number.value => "+(--number.value))
Console::outln("number.value => "+(number.value))
