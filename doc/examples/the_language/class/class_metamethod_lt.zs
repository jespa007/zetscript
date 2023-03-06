class MyNumber{
	constructor(_n){
 		this.num=_n;
	}
 	static _lt(op1, op2){
 		return op1.num==op2.num;
 	}
};

Console::outln("new MyNumber(20) < new MyNumber(20) => "+(new MyNumber(20) < new MyNumber(20)))
Console::outln("new MyNumber(20) < new MyNumber(30) => "+(new MyNumber(20) < new MyNumber(30)))