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
			var init_value=this.__value__;
			++this.__value__;
			return init_value;
		}
		_predec(){
			var init_value=this.__value__;
			--this.__value__;
			return init_value;
		}
		_addassign(_val){
			this.__value__+=_val;
		}
		_subassign(_val){
			this.__value__-=_val;
		}
		_mulassign(_val){
			this.__value__*=_val;
		}
		_divassign(_val){
			this.__value__/=_val;
		}
		_modassign(_val){
			this.__value__%=_val;
		}
		_xorassign(_val){
			this.__value__=Integer::parse(this.__value__)^Integer::parse(_val);
		}
		_orassign(_val){
			this.__value__=Integer::parse(this.__value__)|Integer::parse(_val);
		}
		_andassign(_val){
			this.__value__=Integer::parse(this.__value__)&Integer::parse(_val);
		}
		_shlassign(_val){
			this.__value__=Integer::parse(this.__value__)<<Integer::parse(_val);
		}
		_shrassign(_val){
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
	
	_addassign(_n1){
		if(isNumber(_n1)){
			this.__value__+=_n1
		}else if( _n1 instanceof Number){
			this.__value__+=_n1.__value__	
		}else{
			System::error("Number::_addassign: expected _n1 as Integer,Float or Number but it was '{0}'",typeof _n1)
		}		
	}
	
	static _sub(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseFloat(_n1,_n2);		
		return new Number(n1-n2)
	}
	
	_subassign(_n1){
		if(isNumber(_n1)){
			this.__value__-=_n1
		}else if( _n1 instanceof Number){
			this.__value__-=_n1.__value__	
		}else{
			System::error("Number::_subassign: expected _n1 as Integer,Float or Number but it was '{0}'",typeof _n1)
		}		
	}
	
	static _mul(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseFloat(_n1,_n2);		
		return new Number(n1*n2)
	}
	
	
	_mulassign(_n1){
		if(isNumber(_n1)){
			this.__value__*=_n1
		}else if( _n1 instanceof Number){
			this.__value__*=_n1.__value__	
		}else{
			System::error("Number::_mulassign: expected _n1 as Integer,Float or Number but it was '{0}'",typeof _n1)
		}		
	}
	
	static _div(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseFloat(_n1,_n2);		
		return new Number(n1/n2)
	}
	
	_divassign(_n1){
		if(isNumber(_n1)){
			this.__value__/=_n1
		}else if( _n1 instanceof Number){
			this.__value__/=_n1.__value__	
		}else{
			System::error("Number::_divassign: expected _n1 as Integer,Float or Number but it was '{0}'",typeof _n1)
		}		
	}
	
	static _mod(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseFloat(_n1,_n2);		
		return new Number(n1%n2)
	}
	
	_modassign(_n1){
		if(isNumber(_n1)){
			this.__value__%=_n1
		}else if( _n1 instanceof Number){
			this.__value__%=_n1.__value__	
		}else{
			System::error("Number::_modassign: expected _n1 as Integer,Float or Number but it was '{0}'",typeof _n1)
		}		
	}
	
	static _shr(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseInt(_n1,_n2);		
		return new Number(n1>>n2)
	}
	
	_shrassign(_n1){
		if(isNumber(_n1)){
			this.__value__=Integer::parse(this.__value__)>>Integer::parse(_n1)
		}else if( _n1 instanceof Number){
			this.__value__=Integer::parse(this.__value__)>>Integer::parse(_n1.__value__)	
		}else{
			System::error("Number::_shrassign: expected _n1 as Integer,Float or Number but it was '{0}'",typeof _n1)
		}		
	}

	static _shl(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseInt(_n1,_n2);		
		return new Number(n1<<n2)
	}
	
	_shlassign(_n1){
		if(isNumber(_n1)){
			this.__value__=Integer::parse(this.__value__)<<Integer::parse(_n1)
		}else if( _n1 instanceof Number){
			this.__value__=Integer::parse(this.__value__)<<Integer::parse(_n1.__value__)	
		}else{
			System::error("Number::_shlassign: expected _n1 as Integer,Float or Number but it was '{0}'",typeof _n1)
		}		
	}
	
	static _and(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseInt(_n1,_n2);		
		return new Number(n1&n2)
	}
	
	_andassign(_n1){
		if(isNumber(_n1)){
			this.__value__=Integer::parse(this.__value__)&Integer::parse(_n1)
		}else if( _n1 instanceof Number){
			this.__value__=Integer::parse(this.__value__)&Integer::parse(_n1.__value__)	
		}else{
			System::error("Number::_andassign: expected _n1 as Integer,Float or Number but it was '{0}'",typeof _n1)
		}		
	}
	
	static _or(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseInt(_n1,_n2);		
		return new Number(n1|n2)
	}
	
	_orassign(_n1){
		if(isNumber(_n1)){
			this.__value__=Integer::parse(this.__value__)|Integer::parse(_n1)
		}else if( _n1 instanceof Number){
			this.__value__=Integer::parse(this.__value__)|Integer::parse(_n1.__value__)	
		}else{
			System::error("Number::_orassign: expected _n1 as Integer,Float or Number but it was '{0}'",typeof _n1)
		}		
	}
	
	static _xor(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseInt(_n1,_n2);		
		return new Number(n1^n2)
	}
	
	_xorassign(_n1){
		if(isNumber(_n1)){
			this.__value__=Integer::parse(this.__value__)^Integer::parse(_n1)
		}else if( _n1 instanceof Number){
			this.__value__=Integer::parse(this.__value__)^Integer::parse(_n1.__value__)	
		}else{
			System::error("Number::_xorassign: expected _n1 as Integer,Float or Number but it was '{0}'",typeof _n1)
		}		
	}
	
	static _equ(_n1, _n2){
		var n1,n2
		n1,n2=Number::parseFloat(_n1,_n2);
		return n1==n2
	}
	
	_neg(){
		return new Number(-this.value);
	}
	
	_postinc(){
		return new Number(this.value++);
	}
	
	_postdec(){
		return new Number(this.value--);
	}
	
	_preinc(){
		return new Number(++this.value);
	}
	
	_predec(){
		return new Number(--this.value);
	}
}