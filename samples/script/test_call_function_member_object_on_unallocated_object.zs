var a=null

class A{
	A{
		a=function(){} // creates a member function object and assigns to global var
	}
	
}

function createA(){
	new A()
}

createA();

a(); //<-- cannot called because the object was destroyed