// A class example ...
class A{
	// ... with built-in member vars with optional initialization
	var a,b=-1,c=-2
	const MAX_NUM=9;

	constructor(a,b=-1,c=A::MAX_NUM){
		this.a=a; // this.a member
		this.b=b; // this.b member
		this.c=c;  // this.c member
		this.d=-100; // this.d user property
		this["e"]=-200; // another user property usign []
		this.h=this.f; // 
	}
	
};



// late bind var
var A::f=-1,A::g

var a=new A(1);
System::assert(a.a==1,"a!=1");
System::assert(a.b==-1,"a.b!=-1");
System::assert(a.c==A::MAX_NUM,"a.c!=A::MAX_NUM");
System::assert(a.d==-100,"a.d!=-100");
System::assert(a.e==-200,"a.e!=-200");
System::assert(a.f==-1,"a.f!=-1");
System::assert(a.g==undefined,"a.g!=undefined");
System::assert(a.h==-1,"a.h!=-1");


