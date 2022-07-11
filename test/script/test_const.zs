const c=[34+39,9,0]; // <-- demostrates const var

class A{
	const a=new A();
    const b=11
};

// post declaration as const A::c
const A::c=20

System::assert(A::a instanceof A,"(A::a instanceof A)==false"); 
System::assert(A::b==11,"A::b!=11"); 
System::assert(A::c==20,"A::c!=10");




