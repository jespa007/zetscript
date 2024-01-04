class Number{
	constructor(_value=0){
		this.__value__=_value;
	} 
	
 	_not(){
 		return this.__value__==0 || this.__value__==undefined || this.__value__ == null;
 	}

};

var number=new Number()

if(!number){
	Console::outln("Number is empty")	
}