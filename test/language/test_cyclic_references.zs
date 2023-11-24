
class B{
	constructor(_a_ref){
		this.a_ref=_a_ref; // cyclic reference by object
		this.b=this;
	}

}

class A{
	constructor(){
		this.b=new B(this) // cyclic reference
		this.v=[this,this.b] // cyclic reference
		this.a=this;
		
	}
}


new A()

//System::asset(System::objectRefences()==0,"System::objectRefences()!=0");

