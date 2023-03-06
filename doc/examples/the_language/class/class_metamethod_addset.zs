class MyNumber{
	constructor(_n){
 		this.__num__=_n;
	}
 	_addset(_v){
 	    if(_v instanceof MyNumber){
 	    	this.__num__+=_v.__num__
 	    	return;
 	    }
 		
 		this.__num__+=_v;
 	}
};

var sum=new MyNumber(20);
sum+=20;
Console::outln("op2+=20 => "+sum)
sum+=new MyNumber(30);
Console::outln("op2+=new MyNumber(30) => "+sum)

