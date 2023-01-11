class Number{
	var __num__;
	
	constructor(_num=0){
		this.__num__=0
		if(isNumber(_num)){
			this.__num__=_num
		}else if(_num instanceof Number){
			this.__num__=_num.__num__;	
		}else{
			System::error("Number::constructor: expected _n as Integer,Float or Number but it was '{0}'",typeof _num)
		}
	}
	
	num{
		// input values expects to be simple int or float
		_get(){
			return this.__num__;
		}
		_neg(){
			return -this.__num__;
		}		
		_set(_val){
			this.__num__=_val;
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
		_xor_set(_val){
			this.__num__=Integer::parse(this.__num__)^Integer::parse(_val);
		}
		_or_set(_val){
			this.__num__=Integer::parse(this.__num__)|Integer::parse(_val);
		}
		_and_set(_val){
			this.__num__=Integer::parse(this.__num__)&Integer::parse(_val);
		}
		_shl_set(_val){
			this.__num__=Integer::parse(this.__num__)<<Integer::parse(_val);
		}
		_shr_set(_val){
			this.__num__=Integer::parse(this.__num__)>>Integer::parse(_val);
		}
	}

	static parseInt(_n1,_n2){
		var n1,n2
		if(isNumber(_n1)){
			n1=Integer::parse(_n1)
		}else if(_n1 instanceof Number){
			n1=Integer::parse(_n1.__num__)
		}else{
			System::error("Number::parseInt: expected _n as Integer,Float or Number but it was '{0}'",typeof _n1)
		}
	
		if(isNumber(_n2)){
			n2=Integer::parse(_n2)
		}else if(_n2 instanceof Number){
			n2=Integer::parse(_n2.__num__)	
		}else{
			System::error("Number::parseInt: expected _n as Integer,Float or Number but it was '{0}'",typeof _n2)
		}
	
		return n1,n2
	}
	
	static parseFloat(_n1,_n2){
	var ll=0.5
		Console::outln(ll)
	
		var n1,n2
		if(isNumber(_n1)){
			n1=_n1
		}else if(_n1 instanceof Number){
			n1=_n1.__num__	
		}else{
			System::error("Number::parseFloat: expected _n1 Integer,Float or Number but it was '{0}'",typeof _n1)
		}
	
		if(isNumber(_n2)){
			n2=_n2
		}else if(_n2 instanceof Number){
			n2=_n2.__num__	
		}else{
			System::error("Number::parseFloat: expected _n2 Integer,Float or Number but it was '{0}'",typeof _n2)
		}
	
		return n1,n2
	}
	
	static _add(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseFloat(_n1,_n2);		
		return new Number(n1+n2)
	}
	
	_add_set(_n1){
		if(isNumber(_n1)){
			this.__num__+=_n1
		}else if( _n1 instanceof Number){
			this.__num__+=_n1.__num__	
		}else{
			System::error("Number::_add_set: expected _n1 as Integer,Float or Number but it was '{0}'",typeof _n1)
		}		
	}
	
	static _sub(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseFloat(_n1,_n2);		
		return new Number(n1-n2)
	}
	
	_sub_set(_n1){
		if(isNumber(_n1)){
			this.__num__-=_n1
		}else if( _n1 instanceof Number){
			this.__num__-=_n1.__num__	
		}else{
			System::error("Number::_sub_set: expected _n1 as Integer,Float or Number but it was '{0}'",typeof _n1)
		}		
	}
	
	static _mul(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseFloat(_n1,_n2);		
		return new Number(n1*n2)
	}
	
	
	_mul_set(_n1){
		if(isNumber(_n1)){
			this.__num__*=_n1
		}else if( _n1 instanceof Number){
			this.__num__*=_n1.__num__	
		}else{
			System::error("Number::_mul_set: expected _n1 as Integer,Float or Number but it was '{0}'",typeof _n1)
		}		
	}
	
	static _div(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseFloat(_n1,_n2);		
		return new Number(n1/n2)
	}
	
	_div_set(_n1){
		if(isNumber(_n1)){
			this.__num__/=_n1
		}else if( _n1 instanceof Number){
			this.__num__/=_n1.__num__	
		}else{
			System::error("Number::_div_set: expected _n1 as Integer,Float or Number but it was '{0}'",typeof _n1)
		}		
	}
	
	static _mod(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseFloat(_n1,_n2);		
		return new Number(n1%n2)
	}
	
	_mod_set(_n1){
		if(isNumber(_n1)){
			this.__num__%=_n1
		}else if( _n1 instanceof Number){
			this.__num__%=_n1.__num__	
		}else{
			System::error("Number::_mod_set: expected _n1 as Integer,Float or Number but it was '{0}'",typeof _n1)
		}		
	}
	
	static _shr(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseInt(_n1,_n2);		
		return new Number(n1>>n2)
	}
	
	_shr_set(_n1){
		if(isNumber(_n1)){
			this.__num__=Integer::parse(this.__num__)>>Integer::parse(_n1)
		}else if( _n1 instanceof Number){
			this.__num__=Integer::parse(this.__num__)>>Integer::parse(_n1.__num__)	
		}else{
			System::error("Number::_shr_set: expected _n1 as Integer,Float or Number but it was '{0}'",typeof _n1)
		}		
	}

	static _shl(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseInt(_n1,_n2);		
		return new Number(n1<<n2)
	}
	
	_shl_set(_n1){
		if(isNumber(_n1)){
			this.__num__=Integer::parse(this.__num__)<<Integer::parse(_n1)
		}else if( _n1 instanceof Number){
			this.__num__=Integer::parse(this.__num__)<<Integer::parse(_n1.__num__)	
		}else{
			System::error("Number::_shl_set: expected _n1 as Integer,Float or Number but it was '{0}'",typeof _n1)
		}		
	}
	
	static _and(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseInt(_n1,_n2);		
		return new Number(n1&n2)
	}
	
	_and_set(_n1){
		if(isNumber(_n1)){
			this.__num__=Integer::parse(this.__num__)&Integer::parse(_n1)
		}else if( _n1 instanceof Number){
			this.__num__=Integer::parse(this.__num__)&Integer::parse(_n1.__num__)	
		}else{
			System::error("Number::_and_set: expected _n1 as Integer,Float or Number but it was '{0}'",typeof _n1)
		}		
	}
	
	static _or(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseInt(_n1,_n2);		
		return new Number(n1|n2)
	}
	
	_or_set(_n1){
		if(isNumber(_n1)){
			this.__num__=Integer::parse(this.__num__)|Integer::parse(_n1)
		}else if( _n1 instanceof Number){
			this.__num__=Integer::parse(this.__num__)|Integer::parse(_n1.__num__)	
		}else{
			System::error("Number::_or_set: expected _n1 as Integer,Float or Number but it was '{0}'",typeof _n1)
		}		
	}
	
	static _xor(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseInt(_n1,_n2);		
		return new Number(n1^n2)
	}
	
	_xor_set(_n1){
		if(isNumber(_n1)){
			this.__num__=Integer::parse(this.__num__)^Integer::parse(_n1)
		}else if( _n1 instanceof Number){
			this.__num__=Integer::parse(this.__num__)^Integer::parse(_n1.__num__)	
		}else{
			System::error("Number::_xor_set: expected _n1 as Integer,Float or Number but it was '{0}'",typeof _n1)
		}		
	}
	
	static _equ(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseFloat(_n1,_n2);
		return n1==n2
	}
	
	_post_inc(){
		this.num++;
	}
	
	_post_dec(){
		this.num--;
	}
	
	_pre_inc(){
		++this.num;
	}
	
	_pre_dec(){
		--this.num;
	}
}