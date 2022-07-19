class B{
	var c
	constructor(){
		this.b=0
		this.c=0
		this.f(this.a)
		this.d=this; //<-- ciclic reference !!
		//this.d={}
	}
	f(){
		
	}
}

var t=[0]

function test_cyclic_references(){
	var v=[0]
	var a={}


	a.a=1
	v[0]=0

	var b=new B()
	b.c=0	
	v[0]=b
	
	t[0]=new B()
}

test_cyclic_references();

Console::outln(t[0])

//System::asset(System::objectRefences()==0,"System::objectRefences()!=0");

