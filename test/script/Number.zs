class Number{
	constructor(_n=0){
		this.n=0
		if(isNumber(_n)){
			this.n=parseFloat(_n)
		}else if(_n instanceof Number){
			this.n=_n.n;	
		}else{
			System::error("Number::constructor: expected _n as int,float or Number but it was '{0}'",typeof _n)
		}
	}
	
	static parseFloat(_n1,_n2){
		var n1,n2
		if(isNumber(_n1)){
			n1=parseFloat(_n1)
		}else if(_n1 instanceof Number){
			n1=_n1.n	
		}else{
			System::error("Number::parseFloat: expected _n1 int,float or Number but it was '{0}'",typeof _n1)
		}
	
		if(isNumber(_n2)){
			n2=parseFloat(_n2)
		}else if(_n2 instanceof Number){
			n2=_n2.n	
		}else{
			System::error("Number::parseFloat: expected _n2 int,float or Number but it was '{0}'",typeof _n2)
		}
	
		return n1,n2
	}
	
	static _add(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseFloat(_n1,_n2);		
		return new Integer(n1+n2)
	}
	
	static _sub(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseFloat(_n1,_n2);		
		return new Integer(n1-n2)
	}
	
	static _mul(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseFloat(_n1,_n2);		
		return new Integer(n1*n2)
	}
	
	static _div(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseFloat(_n1,_n2);		
		return new Integer(n1/n2)
	}
	
	static _mod(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseFloat(_n1,_n2);		
		return new Integer(n1%n2)
	}
	
	static _shr(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseFloat(_n1,_n2);		
		return new Integer(n1>>n2)
	}

	static _shl(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseFloat(_n1,_n2);		
		return new Integer(n1<<n2)
	}
	
	static _and(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseFloat(_n1,_n2);		
		return new Integer(n1&n2)
	}
	
	static _or(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseFloat(_n1,_n2);		
		return new Integer(n1|n2)
	}
	
	static _xor(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseFloat(_n1,_n2);		
		return new Integer(n1^n2)
	}
	
	static _equ(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseFloat(_n1,_n2);
		return n1==n2
	}

}