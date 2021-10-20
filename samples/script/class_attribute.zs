class A{
	A(){
		this.__a__=10;
		this.__b__=10;
	}
	
	_set(_a){
		Console::outln("assign this.__a__="+_a)
		this.__a__=_a
	}
	
	static _equ(_obj1,_obj2){
		
		Console::outln("_obj1.__a__={0}",_obj1.__a__)
		Console::outln("_obj2.__a__={0}",_obj2)
		
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
			return this.__b__;
		}
	}
}



var a=new A();
a=1;
var c=a.__a__
System::assert(a==2,"a!=1 (a:{0})",c)
a.a=2;
System::assert(a.a==1,"a.a!=2");
a.b=3
System::assert(a.a==2,"a.b!=3");
a=4;
System::assert(a==2,"a!=4")
a.a=5;
System::assert(a.a==4,"a.a!=5");
a.b=6
System::assert(a.a==5,"a.b!=6");


