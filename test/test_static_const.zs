//var c=1;
//const c=[34+39,9,0];

//c=1; // <-- cannot assign constant (run-time error ?!?)

class A{
	//static var a=10 // initialized static var
	//static var b // unitialized static var
	const b=11
	const a=new A(); // a.jjjj();
	//static var v=11;
	
	/*constructor(){
		//this.a=0; // incorrect -> you should use A::a		
	}*/
	
	/*static static_fun(){
		//var a=0;
		//System.print("from static fun")
	}*/
	
	/*print2(){
		
	}*/
	
};



//System::println("gdgdf")
System::println(A::a.b);
A::a=0;
//A::a.print2();


