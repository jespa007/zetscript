class A{
	static a=10;
	const b=11;
	
	constructor(){
		this.a=0; // incorrect -> you should use A::a		
	}
	
	static jjjj(){
		
	}
	
	
};


static A::print(){
	System.println("hola ...");
}

A::b(){
	this.m=0;
}

static A  ::   c=0;
const A::d; // <-- invalid should initialize by =