class A{
	//static var a=10 // initialized static var
	//static var b // unitialized static var
	const b=11
	const a=new A(); // a.jjjj();
	//static var v=11;
	
	constructor(){
		this.a=0; // incorrect -> you should use A::a		
	}
	
	static jjjj(){
		
	}
	
	print2(){
		System.print("print2")
	}
	
	
};

function a(){
	
}


/*static function A::print(){
	System.println("hola ...");
}*/


function A::b(){
	this.m=0;
}

//static var A  ::   c
//const A::d // <-- invalid should initialize by =