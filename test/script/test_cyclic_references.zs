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

function test_cyclic_references(){
	var v=[0]
	var a={}


	a.a=1
	v[0]=0

	var b=new B()
	b.c=0	
}

test_cyclic_references();

System::asset(System::objectRefences()==0,"System::objectRefences()!=0");

