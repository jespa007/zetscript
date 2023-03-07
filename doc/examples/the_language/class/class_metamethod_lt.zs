class Number{
	constructor(_n){
 		this.num=_n;
	}
 	static _lt(op1, op2){
 		return op1.num==op2.num;
 	}
};

Console::outln("new Number(20) < new Number(20) => "+(new Number(20) < new Number(20)))
Console::outln("new Number(20) < new Number(30) => "+(new Number(20) < new Number(30)))