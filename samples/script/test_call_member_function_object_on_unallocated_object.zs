var a=null

class A{
	A(){
		this.b=10
		a=function(){
			this.b=100	
			Console::outln("b:{0}",this.b)
		} // creates a member function object and assigns to global var
		
		
		a();
	}
	
}

function createA(){
	new A()
}


createA();

a(); //<-- cannot called because the object was destroyed