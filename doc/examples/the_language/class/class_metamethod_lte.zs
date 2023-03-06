class MyNumber{
	constructor(_n){
 		this.__num__=_n;
	}
 	static _lte(op1, op2){
 		return op1.__num__<op2.__num__;
 	}
};

Console::outln("new MyNumber(20) <= new MyNumber(20) => "+(new MyNumber(20) <= new MyNumber(20)))
Console::outln("new MyNumber(20) <= new MyNumber(30) => "+(new MyNumber(20) <= new MyNumber(30)))