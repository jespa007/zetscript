class A{
	A(){
		this.__a__=10;
		this.__b__=10;
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
	}
}


//var h=0
/*
function c(ref a){
	System::println(a);
}

function b(ref a){
	//var h=0
	c(a)
	//System::println(a);	
}
*/
var a=new A();
//b(a)
System::println(a.a)
a.a=1;
System::println(a.a);


