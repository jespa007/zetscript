class A{
	A(){
		this.__a__=10;
		this.__b__=10;
	}
	
	_set(_a){
		Console::outln("assign this.__a__="+_a)
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
	}
}



var a=new A();
a=2;
Console::outln(a)
a.a=1;
Console::outln(a.a);
a=2
Console::outln(a.a);


