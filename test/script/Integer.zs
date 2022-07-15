class Integer{
	constructor(_n=0){
		this.n=0
		if(isNumber(_n)){
			this.n=parseInteger(_n)
		}else if(_n instanceof Integer){
			this.n=_n.n;	
		}else{
			System::error("Integer::constructor: expected _n as int,float or Integer but it was '{0}'",typeof _n)
		}
	}
	
	static parseInteger(_n1,_n2){
		var n1,n2
		if(isNumber(_n1)){
			n1=parseInteger(_n1)
		}else if(_n1 instanceof Integer){
			n1=_n1.n	
		}else{
			System::error("Integer::constructor: expected _n as int,float or Integer but it was '{0}'",typeof _n1)
		}
	
		if(isNumber(_n2)){
			n2=parseInteger(_n2)
		}else if(_n2 instanceof Integer){
			n2=_n2.n	
		}else{
			System::error("Integer::constructor: expected _n as int,float or Integer but it was '{0}'",typeof _n2)
		}
	
		return n1,n2
	}
	
	static _add(_n1, _n2){
		var n1,n2
		n1,n2=Integer::parseInteger(_n1,_n2);		
		return new Integer(n1+n2)
	}
	
	static _sub(_n1, _n2){
		var n1,n2
		n1,n2=Integer::parseInteger(_n1,_n2);		
		return new Integer(n1-n2)
	}
	
	static _mul(_n1, _n2){
		var n1,n2
		n1,n2=Integer::parseInteger(_n1,_n2);		
		return new Integer(n1*n2)
	}
	
	static _div(_n1, _n2){
		var n1,n2
		n1,n2=Integer::parseInteger(_n1,_n2);		
		return new Integer(n1/n2)
	}
	
	static _mod(_n1, _n2){
		var n1,n2
		n1,n2=Integer::parseInteger(_n1,_n2);		
		return new Integer(n1%n2)
	}
	
	static _shr(_n1, _n2){
		var n1,n2
		n1,n2=Integer::parseInteger(_n1,_n2);		
		return new Integer(n1>>n2)
	}

	static _shl(_n1, _n2){
		var n1,n2
		n1,n2=Integer::parseInteger(_n1,_n2);		
		return new Integer(n1<<n2)
	}
	
	static _and(_n1, _n2){
		var n1,n2
		n1,n2=Integer::parseInteger(_n1,_n2);		
		return new Integer(n1&n2)
	}
	
	static _or(_n1, _n2){
		var n1,n2
		n1,n2=Integer::parseInteger(_n1,_n2);		
		return new Integer(n1|n2)
	}
	
	static _xor(_n1, _n2){
		var n1,n2
		n1,n2=Integer::parseInteger(_n1,_n2);		
		return new Integer(n1^n2)
	}
	
	static _equ(_n1, _n2){
		var n1,n2
		n1,n2=Integer::parseInteger(_n1,_n2);
		return n1==n2
	}

}