class MyNumber{
	constructor(_n){
 		this.num=_n;
	}
 	_string(){
 		return "Num: "+this.num;
 	}
};

var n=new MyNumber(10)
Console::outln(n)