class Number{
	constructor(_value=0){
		this.__value__=_value;
	}
	
 	static _nequ(op1, op2){
 		return op1.__value__!=op2.__value__;
 	}
};

Console::outln("new Number(20) != new Number(20) => "+(new Number(20) != new Number(20)))
Console::outln("new Number(20) != new Number(30) => "+(new Number(20) != new Number(30)))