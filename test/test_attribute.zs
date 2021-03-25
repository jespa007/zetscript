class A{
	/*A(){
		this.__a__=0;
		this.__b__=0;
	}
	
	_set(_a){
		System::println("assign this.__a__="+_a)
		this.__a__=_a
	}
	
	// attribute
	a{
		_set(value){
			this.__a__=value;
		}
		_get(){
			return this.__a__;
		}
	}	
	
	b{
		_set(value){
			this.__b__=value;
		}
		_get(){
			return __b__;
		}
	}*/		
}


function c(a){
	System::println(a);
}

function b(){
	c(a)	
}

var a=new A();
b()

/*a.a=1;
System::println(a.a);*/


