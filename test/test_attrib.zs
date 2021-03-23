class A{
	A(){
		this.__a__=0;
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
}

var a=new A();
System::println(a.attrib);
a.a=1;
System::println(a.attrib);


