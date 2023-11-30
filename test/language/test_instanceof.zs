class A{

}

class B extends A{

	
}

class C{
	
}


System::assert(0 instanceof Integer==true,"0 instanceof Integer!=true")
System::assert(0.0 instanceof Float==true,"0.0 instanceof Float!=true")
System::assert(true instanceof Boolean==true,"true instanceof Boolean!=true")
System::assert([0,1,2] instanceof Array==true,"[0,1,2] instanceof Array!=true")
System::assert("abc" instanceof String==true,"abc instanceof String!=true")
System::assert(new A() instanceof A == true,"new A() instanceof A != true")
System::assert(new B() instanceof A == true,"new B() instanceof A != true")
System::assert(new B() instanceof B ==true,"new B() instanceof B !=true")
System::assert(new C() instanceof B == false,"new C() instanceof B != false")
System::assert({a:0,b:1} instanceof ScriptObject == true,"{a:0,b:1} instanceof ScriptObject != true")
	



