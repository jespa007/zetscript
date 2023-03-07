class Number{
	constructor(_n){
 		this.__num__=_n;
	}
 	static _gt(op1, op2){
 		return op1.__num__>op2.__num__;
 	}
};

Console::outln("new Number(20) > new Number(20) => "+(new Number(20) > new Number(20)))
Console::outln("new Number(20) > new Number(30) => "+(new Number(20) > new Number(30)))