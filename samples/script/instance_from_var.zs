class A{
	A(){
		Console::outln("instance from A")
	}
}

// it loads type of A into variable type_a
var Object=A;


// it can create instances from vars that contains a type
new type_a();

