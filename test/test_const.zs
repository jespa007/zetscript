const c=[34+39,9,0]; // <-- demostrates const var

class A{
    const b=11
	const a=new A();

};

Console::outln(A::b); // <-- demostrates static access const var
Console::outln(A::a); // <-- demostrates static access const allocated class
Console::outln(A::a.a); //<-- the constant 'a' itself




