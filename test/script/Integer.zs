class Integer{
	
	var __num__;
	
	constructor(_num=0){
		this.__num__=0
		if(isNumber(_num)){
			this.__num__=parseInteger(_num)
		}else if(_num instanceof Integer){
			this.__num__=_num.__num__;	
		}else{
			System::error("Integer::constructor: expected _n as int,float or Integer but it was '{0}'",typeof _num)
		}
	}
	
	num{
		_get(){
			return this.__num__;
		}
		_set(_val){
			this.__num__=_val;
		}
		_neg(){
			return -this.__num__=_val;
		}
		_post_inc(){
			this.__num__++;
		}
		_post_dec(){
			this.__num__--;
		}
		_pre_inc(){
			++this.__num__;
		}
		_pre_dec(){
			--this.__num__;
		}
		_add_set(_val){
			this.__num__+=_val;
		}
		_sub_set(_val){
			this.__num__-=_val;
		}
		_mul_set(_val){
			this.__num__*=_val;
		}
		_div_set(_val){
			this.__num__/=_val;
		}
		_mod_set(_val){
			this.__num__%=_val;
		}
		_or_set(_val){
			this.__num__|=_val;
		}
		_and_set(_val){
			this.__num__&=_val;
		}
		_xor_set(_val){
			this.__num__^=_val;
		}
		_shr_set(_val){
			this.__num__>>=_val;
		}
		_shl_set(_val){
			this.__num__<<=_val;
		}
	}

	
	static parseInteger(_n1,_n2){
		var n1,n2
		if(isNumber(_n1)){
			n1=parseInteger(_n1)
		}else if(_n1 instanceof Integer){
			n1=_n1.__num__	
		}else{
			System::error("Integer::constructor: expected _n as int,float or Integer but it was '{0}'",typeof _n1)
		}
	
		if(isNumber(_n2)){
			n2=parseInteger(_n2)
		}else if(_n2 instanceof Integer){
			n2=_n2.__num__	
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