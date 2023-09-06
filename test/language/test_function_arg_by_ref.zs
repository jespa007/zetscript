class A{}
class B{}

var a=10,c=new A();

// function WITHOUT ref
function(_a,_c){
	_a=0;
	_c=new B();
}(a,c)

System::assert(a==10,"a!=10")
System::assert(c instanceof A,"!(c instanceof A)")

a=10
c=new A();

// function WITH ref
function(ref _a, ref _c){
	_a=0;
	var b=_c;
	_c=new B(); // _c write reg
}(a,c)


System::assert(a==0,"a!=0")
System::assert(c instanceof B,"!(c instanceof B)")




