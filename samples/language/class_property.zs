class A{

	// attributes
	a{
		_set(value){
			this.__attrib_a__=value;
		}
		_get(){
			return this.__attrib_a__;
		}
	}	
	
	b{
		_set(value){
			this.__attrib_b__=value;
		}
		_get(){
			return this.__attrib_b__;
		}
	}
}



var a=new A();
a.a=1;
System::assert(a.a==1,"a.a!=1");
a.b=2
System::assert(a.b==2,"a.b!=2");


