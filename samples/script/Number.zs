class Number{
	constructor(){
		this.n=0;
	}
	
	static getNumbers(_n1,_n2){
		var n1,n2
		if(isNumber(_n1)){
			n1=parseFloat(_n1)
		}else if(_n1 instanceof Number){
			n1=_n1.n	
		}
		if(isNumber(_n2)){
			n2=parseFloat(_n2)
		}else if(_n2 instanceof Number){
			n2=_n2.n	
		}
		return n1,n2
	}
	
	static _add(_n1, _n2){
		var n1,n2
		n1,n2=Number::getNumbers(_n1,_n2);		
		return new Number(n1+n2)
	}

}