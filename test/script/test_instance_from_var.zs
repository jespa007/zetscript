class A{
	A(){
	}
}

// it loads type of A into variable type_a
var type_a=A;


// it can create instances from vars that contains a type
new type_a();

System::assert(type_a == A,"type_a != typeof A")

