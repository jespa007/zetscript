class Number{
	constructor(_value=0){
		// ensure that n is literal integer or literal float
		if(_value instanceof Integer || _value instanceof Float){
			this.__value__=_value;
		}else{
			System::error("Number::constructor : input value not supported");
		}
	} 
	
	_preinc(){
		this.__value__+=1;
	}
	
 	_tostring(){
 		return this.__value__;
 	}
}

var number=new Number(20);
Console::outln("++number => "+(++number))
Console::outln("number => "+(number))
