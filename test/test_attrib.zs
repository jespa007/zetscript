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


function b(ref a){
	System::println(a.a);	
}

var a=new A();
b(a)

/*a.a=1;
System::println(a.a);*/


