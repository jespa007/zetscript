class MyNumber{
	constructor(){
 		this.num=0;
	} 
 	_set(v){
 		if(v instanceof Integer){
 			this.num = v;
 		}else if(v instanceof MyNumber){
 			this.num = v.num;
 		}else{
 			error("parameter not supported");
 		}
 	}
 	_toString(){
 		return "Num:"+this.num;
 	}
};

var n = new MyNumber ();
Console::outln(n)
n=10;
Console::outln(n)
