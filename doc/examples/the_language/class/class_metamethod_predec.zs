class Number{
	constructor(_value=0){
		this.__value__=_value;
	} 
	
	_predec(){
		return new Number(--this.__value__);
	}
	
 	_tostring(){
 		return this.__value__;
 	}
}

var number=new Number(20);
Console::outln("--number => "+(--number))
Console::outln("number => "+(number))
