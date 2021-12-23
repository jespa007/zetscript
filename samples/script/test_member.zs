// A class example
class A{
	// built-in member vars with optional initialization
	var a,b=-1,c=-2

	Test(a,b=-1,c=Test::MAX_NUM){
		this.a=a; // this.a member
		this.b=b; // this.b member
		this.c=c;  // this.c member
		this.d=-100; // this.d user property
		this["e"]=-200; // another user property usign []
		Console::outln("A::A this.a:{0} this.b:{1} this.c:{2} this.d:{3} this.e:{4} this.f:{5}"
				,this["a"]
				,this.b
				,this.c
				,this.d
				,this.e
				,this.f
		);
		
	}

};



// late bind var and const member
var A::f=-1,A::e

var a=new A(1);
 
System::assert(a==1,"a!=1");

