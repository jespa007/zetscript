class Number{
	constructor(_value=0){
		this.__value__=_value;
	} 
	
	// property 'value'
	value{
		_get(){
			return this.__value__
		}
	}
}

var number=new Number(20);
Console::outln("number.value => "+number.value)
