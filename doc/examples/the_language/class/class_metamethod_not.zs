class Number{
	constructor(_value=0){
		// ensure that n is literal integer or literal float
		if(_value instanceof Integer || _value instanceof Float){
			this.__num__=_value;
		}else{
			System::error("Number::constructor : input value not supported");
		}
	} 
	
 	_not(){
 		return this.__num__==0 || this.__num__==undefined || this.__num__ == null;
 	}

};

var number=new Number()

if(!number){
	Console::outln("Number is empty")	
}