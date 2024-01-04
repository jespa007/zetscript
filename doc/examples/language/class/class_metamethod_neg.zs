class Number{
	constructor(_value=0){
		this.__value__=_value;
	} 
	
 	_neg(){
 		return -this.__value__;
 	}

};

var number=new Number(20)

Console::outln("-number => "+ (-number))

