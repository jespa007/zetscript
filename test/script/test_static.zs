class A{
	static a=new A();
    static b=11
	    
    A(){
    	Console::outln("a:{0} b:{1} c:{2}",A::a,A::b,A::c)
    }

};

// post declaration as const A::c
static A::c=20

System::assert(A::a instanceof A,"(A::a instanceof A)==false"); 
System::assert(A::b==11,"A::b!=11"); 
System::assert(A::a.a instanceof A,"(A::a.a instanceof A)==false");
System::assert(A::c==20,"A::c!=10");




