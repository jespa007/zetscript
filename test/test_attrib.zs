class A{
	A(){
		this.__a__=0;
		this.__b__=0;
	}
	
	// attribute
	a{
		_set(value){
			this.__a__=value;
		}
		_get(){
			return __a__;
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
System::println(a.a);
a.a=1;
System::println(a.a);


