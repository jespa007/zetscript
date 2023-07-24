// C : Implement postinc with no return
class C{
	constructor(_value){
		this.__value__=_value;
	}
	
	_postinc(){
		this.__value__++;
	}
}

// D : Implement postinc with return
class D{
	constructor(_value){
		this.value=_value;
	}
	
	static _equ(_a,_b){
		var a=_a;
		var b=_b;
		if(a instanceof D){
			a=a.value;
		}
		if(b instanceof D){
			b=b.value;
		}
		
		return a==b;
	}
	
	_neg(){
		return new D(-this.value)
	}
	
	_bwc(){
		return new D(~this.value)
	}	
	
	_postinc(){
		var n=new D(this.value);
		this.value++;
		return n;
	}
}


// E : Property postinc WITH NO return
class E{
	constructor(_value){
		this.__value__=_value;
	}
	
	value{
		_get(){
			return this.__value__;
		}
		_postinc(){
			this.__value__++;
		}
	}
}

// F : Property postinc WITH return
class F{
	constructor(_value){
		this.__value__=_value;
	}
	
	value{
		_postinc(){
			var n=this.__value__;
			this.__value__++;
			return n;
		}
	}
}

var a,b,c,d,e,f;

a=2;

// basic
// read only
System::assert(2==a++,"2!=a++");
System::assert(-3==-a++,"-3!=-a++");
System::assert(~4==~a++,"~4!=~a++");


// assignment
b=2;

a=b++;
System::assert(2==a,"a=b++ => 2!=a");

a=-b++;
System::assert(-3==a,"a=-b++ => -3!=-a");

a=~b++;
System::assert(~4==a,"a=~b++ => ~4!=~a");


// test post in WITH NO return 
c=new C(2);
c++;
System::assert(3==c.__value__,"c++ => 3!=c.__value__");

a=c++;
System::assert(undefined==a,"a=c++ => undefined!=a");

// test post in WITH return
d=new D(2);
a=-d++;
System::assert(-2==a,"a=-d++ => -2!=a");

a=~d++;
System::assert(~3==a,"a=~d++ => ~3!=a");

// test post in WITH NO return 
e=new E(2);
e.value++;
System::assert(3==e.value,"e.value++ => 3!=e.value");

a=e.value++;
System::assert(undefined==a,"a=e.value++ => undefined!=a");

// test post in WITH return
f=new F(2);
a=-f.value++;
System::assert(-2==a,"-f.value++ => -2!=a");

a=~f.value++;
System::assert(~3==a,"~f.value++ => ~3!=a");

