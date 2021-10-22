class A{
	A(){
		this.__a__=10;
		this.__b__=10;
	}
	
	_set(_a){
		this.__a__=_a
	}
	
	static _equ(_obj1,_obj2){
		
		
		if((_obj1 instanceof A)
				   &&
		  !(_obj2 instanceof A)
				){
			
			return _obj1.__a__ == _obj2;	
		}
		else if(!(_obj1 instanceof A)
			&&
			(_obj2 instanceof A)
		){
			return _obj1==_obj2.__a__;
		}else if(_obj1 instanceof A && _obj2 instanceof A){
			return _obj1.__a__==_obj2.__a__;
		}
		
		System::error("I don't how to compare {0} and {1}",typeof _obj1, typeof _obj2)
				
	}
	
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
a=1;
System::assert(a==1,"a!=1")
a.a=2;
System::assert(a.a==2,"a.a!=2");
a.b=3
System::assert(a.b==3,"a.b!=3");


