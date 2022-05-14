class A{

}

class B extends A{

	
}

class C{
	
}


System::assert(0 instanceof int==true,"0 instanceof int!=true")
System::assert(0.0 instanceof float==true,"0.0 instanceof float!=true")
System::assert(true instanceof bool==true,"true instanceof bool!=true")
System::assert([0,1,2] instanceof Vector==true,"[0,1,2] instanceof Vector!=true")
System::assert("abc" instanceof String==true,"abc instanceof String!=true")
System::assert(new A() instanceof A == true,"new A() instanceof A != true")
System::assert(new B() instanceof A == true,"new B() instanceof A != true")
System::assert(new B() instanceof B ==true,"new B() instanceof B !=true")
System::assert(new C() instanceof B == false,"new C() instanceof B != false")
System::assert({a:0,b:1} instanceof Object == true,"{a:0,b:1} instanceof Object != true")
	



