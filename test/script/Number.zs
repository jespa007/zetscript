class Number{
	var __value__;
	
	constructor(_num=0){
		this.__value__=0
		if(isNumber(_num)){
			this.__value__=_num
		}else if(_num instanceof Number){
			this.__value__=_num.__value__;	
		}else{
			System::error("Number::constructor: expected _n as Integer,Float or Number but it was '{0}'",typeof _num)
		}
	}
	
	value{
		// input values expects to be simple int or float
		_get(){
			return this.__value__;
		}
		_neg(){
			return -this.__value__;
		}		
		_set(_val){
			this.__value__=_val;
		}
		_postinc(){
			var init_value=this.__value__;
			this.__value__++;
			return init_value;
		}
		_postdec(){
			var init_value=this.__value__;
			this.__value__--;
			return init_value;
		}
		_preinc(){
			++this.__value__;
		}
		_predec(){
			--this.__value__;
		}
		_addset(_val){
			this.__value__+=_val;
		}
		_subset(_val){
			this.__value__-=_val;
		}
		_mulset(_val){
			this.__value__*=_val;
		}
		_divset(_val){
			this.__value__/=_val;
		}
		_modset(_val){
			this.__value__%=_val;
		}
		_xorset(_val){
			this.__value__=Integer::parse(this.__value__)^Integer::parse(_val);
		}
		_orset(_val){
			this.__value__=Integer::parse(this.__value__)|Integer::parse(_val);
		}
		_andset(_val){
			this.__value__=Integer::parse(this.__value__)&Integer::parse(_val);
		}
		_shlset(_val){
			this.__value__=Integer::parse(this.__value__)<<Integer::parse(_val);
		}
		_shrset(_val){
			this.__value__=Integer::parse(this.__value__)>>Integer::parse(_val);
		}
	}

	static parseInt(_n1,_n2){
		var n1,n2
		if(isNumber(_n1)){
			n1=Integer::parse(_n1)
		}else if(_n1 instanceof Number){
			n1=Integer::parse(_n1.__value__)
		}else{
			System::error("Number::parseInt: expected _n as Integer,Float or Number but it was '{0}'",typeof _n1)
		}
	
		if(isNumber(_n2)){
			n2=Integer::parse(_n2)
		}else if(_n2 instanceof Number){
			n2=Integer::parse(_n2.__value__)	
		}else{
			System::error("Number::parseInt: expected _n as Integer,Float or Number but it was '{0}'",typeof _n2)
		}
	
		return n1,n2
	}
	
	static parseFloat(_n1,_n2){
		var n1,n2
		if(isNumber(_n1)){
			n1=_n1
		}else if(_n1 instanceof Number){
			n1=_n1.__value__	
		}else{
			System::error("Number::parseFloat: expected _n1 Integer,Float or Number but it was '{0}'",typeof _n1)
		}
	
		if(isNumber(_n2)){
			n2=_n2
		}else if(_n2 instanceof Number){
			n2=_n2.__value__	
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
	
	_addset(_n1){
		if(isNumber(_n1)){
			this.__value__+=_n1
		}else if( _n1 instanceof Number){
			this.__value__+=_n1.__value__	
		}else{
			System::error("Number::_addset: expected _n1 as Integer,Float or Number but it was '{0}'",typeof _n1)
		}		
	}
	
	static _sub(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseFloat(_n1,_n2);		
		return new Number(n1-n2)
	}
	
	_subset(_n1){
		if(isNumber(_n1)){
			this.__value__-=_n1
		}else if( _n1 instanceof Number){
			this.__value__-=_n1.__value__	
		}else{
			System::error("Number::_subset: expected _n1 as Integer,Float or Number but it was '{0}'",typeof _n1)
		}		
	}
	
	static _mul(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseFloat(_n1,_n2);		
		return new Number(n1*n2)
	}
	
	
	_mulset(_n1){
		if(isNumber(_n1)){
			this.__value__*=_n1
		}else if( _n1 instanceof Number){
			this.__value__*=_n1.__value__	
		}else{
			System::error("Number::_mulset: expected _n1 as Integer,Float or Number but it was '{0}'",typeof _n1)
		}		
	}
	
	static _div(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseFloat(_n1,_n2);		
		return new Number(n1/n2)
	}
	
	_divset(_n1){
		if(isNumber(_n1)){
			this.__value__/=_n1
		}else if( _n1 instanceof Number){
			this.__value__/=_n1.__value__	
		}else{
			System::error("Number::_divset: expected _n1 as Integer,Float or Number but it was '{0}'",typeof _n1)
		}		
	}
	
	static _mod(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseFloat(_n1,_n2);		
		return new Number(n1%n2)
	}
	
	_modset(_n1){
		if(isNumber(_n1)){
			this.__value__%=_n1
		}else if( _n1 instanceof Number){
			this.__value__%=_n1.__value__	
		}else{
			System::error("Number::_modset: expected _n1 as Integer,Float or Number but it was '{0}'",typeof _n1)
		}		
	}
	
	static _shr(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseInt(_n1,_n2);		
		return new Number(n1>>n2)
	}
	
	_shrset(_n1){
		if(isNumber(_n1)){
			this.__value__=Integer::parse(this.__value__)>>Integer::parse(_n1)
		}else if( _n1 instanceof Number){
			this.__value__=Integer::parse(this.__value__)>>Integer::parse(_n1.__value__)	
		}else{
			System::error("Number::_shrset: expected _n1 as Integer,Float or Number but it was '{0}'",typeof _n1)
		}		
	}

	static _shl(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseInt(_n1,_n2);		
		return new Number(n1<<n2)
	}
	
	_shlset(_n1){
		if(isNumber(_n1)){
			this.__value__=Integer::parse(this.__value__)<<Integer::parse(_n1)
		}else if( _n1 instanceof Number){
			this.__value__=Integer::parse(this.__value__)<<Integer::parse(_n1.__value__)	
		}else{
			System::error("Number::_shlset: expected _n1 as Integer,Float or Number but it was '{0}'",typeof _n1)
		}		
	}
	
	static _and(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseInt(_n1,_n2);		
		return new Number(n1&n2)
	}
	
	_andset(_n1){
		if(isNumber(_n1)){
			this.__value__=Integer::parse(this.__value__)&Integer::parse(_n1)
		}else if( _n1 instanceof Number){
			this.__value__=Integer::parse(this.__value__)&Integer::parse(_n1.__value__)	
		}else{
			System::error("Number::_andset: expected _n1 as Integer,Float or Number but it was '{0}'",typeof _n1)
		}		
	}
	
	static _or(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseInt(_n1,_n2);		
		return new Number(n1|n2)
	}
	
	_orset(_n1){
		if(isNumber(_n1)){
			this.__value__=Integer::parse(this.__value__)|Integer::parse(_n1)
		}else if( _n1 instanceof Number){
			this.__value__=Integer::parse(this.__value__)|Integer::parse(_n1.__value__)	
		}else{
			System::error("Number::_orset: expected _n1 as Integer,Float or Number but it was '{0}'",typeof _n1)
		}		
	}
	
	static _xor(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseInt(_n1,_n2);		
		return new Number(n1^n2)
	}
	
	_xorset(_n1){
		if(isNumber(_n1)){
			this.__value__=Integer::parse(this.__value__)^Integer::parse(_n1)
		}else if( _n1 instanceof Number){
			this.__value__=Integer::parse(this.__value__)^Integer::parse(_n1.__value__)	
		}else{
			System::error("Number::_xorset: expected _n1 as Integer,Float or Number but it was '{0}'",typeof _n1)
		}		
	}
	
	static _equ(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseFloat(_n1,_n2);
		return n1==n2
	}
	
	_postinc(){
		this.value++;
	}
	
	_postdec(){
		this.value--;
	}
	
	_preinc(){
		++this.value;
	}
	
	_predec(){
		--this.value;
	}
}