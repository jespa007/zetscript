class Number{
	constructor(_value=0){
		this.__value__=_value;
	} 
	
 	_bwc(){
 		return ~Integer::parse(this.__value__);
 	}

};

var number=new Number(2)

Console::outln("~number => "+ (~number))

