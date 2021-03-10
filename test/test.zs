/*System::println(0)*/
var v=[0]
System::println(v[0])
//var s=v.join();

/*for(var i=0; i < v.length; i++){
	System::println(v[i])
}*/

/*var a="a";
var b=1;
System::println(a+b+a)*/
/*
var v=[0,1,2,3]

for(var i=0; i < v.length; ++i){
	System::println(i)
}
*/

/*var v3=[1,2,3]+[4,5,6]

for(var i=0; i < v3.length; i++){
	System::println(v3[i]);
}*/
/*	System::println(v3[0]);
System::println(v3[1]);
System::println(v3[2]);
System::println(v3[3]);
System::println(v3[4]);
System::println(v3[5]);
System::println(v3[6]);*/
//}

/*
class A{
	
}

class B{
	
}

class C{
	
}
*/
/*
function fun2(ref a){
	
	a=5 // <-- should not save
}

function fun(ref a){
	//var b=0
	//a=[]
	a=20
	
	//System::println(a)
	
	fun2(a);
	
	System::println(a)
	
	return a,a;
	
	//a=10 //new A()

}

var a={}*/

//System::println(a)	

//System::println("{0} {1}",0)


//System::println(a)
//System::println(a)

//System::println(a)	

//String::format("dsda")
//Float::parse(2)

/*var a=1

var i1=10/2 // <-- should be decimal instead of in
var i2=Integer.parse(10/2) // <-- convert to int
//Integer.parse("10") // <-- convert to int
//Float.parse("10") //<-- convert to float
*/
/*var a=2;

var i3=100.0/2 // <-- fails
var i4=3110.0/a // <-- fails
*/
//System::println(i3)

/*
var a=[];

function fun(a={}){
	//a={}
	a=[] //<-- here I allocate [] at fun scope but a {} allocated at global scope
	//System::println(a);
} //<-- here a cannot deallocate [] because it's a scope

//System::println(a);

fun(a);*/

//System::println(a);

/*for(;;){
	System::println("loop")
}*/

/*class Test{
	var a=0;

	Test(){
		System::println("dasdas {0}"+this.a);
	}
	
	
	
}*/
/*
class TestEx extends Test{
	TestEx(){
		super(); //<--fails
	}
}

new Test()*/
/*
var a
System::println("dasdas "+a);*/

/*
// A class example
class Test{
	const MAX_NUM=9
	
	// pre create vars with initialization
	var b=-1,c=-2,d="dasds"

	Test(a,b=-1,c=Test::MAX_NUM){
		this.a=a; // init a (dynamic)
		this.b=b
		this.c=c
		System::println("Test::constructor this.a:{0} this.b:{1} this.c:{2} this.d:{3} this.e:{4} this.f:{5} MAX_NUM:{6}"
				,this.a
				,this.b
				,this.c
				,this.d
				,this.e
				,this.f
				,Test::MAX_NUM);
		
	}
	function1(a){
		this.a+=10+a;
		System::println("Test::function1, this.a:{0}",this.a);
		this.function0();
	}
};

// post-include function member
function Test::function2(a){
	this.function1(a+5); // it calls Test::function1(a+5)
	this.a+=a;
	System::println("Test::function2, this.a:{0}",this.a);
}

// post-include var and const member
var Test::f=-1,Test::e,Test::h
const Test::MAX_NUM_EXT=10
*/

//var a=new Test(121)


/*
// A inheritance class example. TestExtended inherites function1 and function2. 
class TestExtended extends Test{
	
	TestExtended(a){
		System::println("TestExtended::constructor this.a:{0}",this.a);
		super(a); // it calls base constructor (by default it doesn't call)
		this.function3=function(a){ // creates anonymous function with this context
			this.a+=a;
			System::println("TextExtended::Anonymous function, this.a:{0}",this.a);
			this.function2(a);
		};
	}
	
	function0(a){
		this.a+=10; 	
		System::println("TestExtended::function0, this.a:{0}",this.a);
	}
	
	function2(a){
		super(a+10); 		// it calls Test::function1(2)
		this.a+=5; 	
		System::println("TestExtended::function2, this.a:{0}",this.a);

	}
};
*/
//var t=new Test(1); 	// instances TestExtended class
//t.function3(20); // it calls anynomus function



/*class Test{
	const MAX_NUM=9
	
	// pre create vars with initialization
	var b=-1,c=-2

	Test(a,b=-1,c=Test::MAX_NUM){
		this.a=a; // init a (dynamic)
		this.b=b
		this.c=c
		System::println("Test::constructor this.a:{0} this.b:{1} this.c:{2} this.d:{3} this.e:{4} this.f:{5} MAX_NUM:{6}"
				,this.a
				,this.b
				,this.c
				,this.d
				,this.e
				,this.f
				,Test::MAX_NUM
				);
		
	}
	function1(a){
		this.a+=10+a;
		System::println("Test::function1, this.a:{0}",this.a);
		this.function0();
	}
};
var Test::f=-1,Test::e
const Test::MAX_NUM_EXT=10
//post-include function member
function Test::function2(a){
	this.function1(a+5); // it calls Test::function1(a+5)
	this.a+=a;
	System::println("Test::function2, this.a:{0}",this.a);
}

// post-include var and const member
//var Test::f=-1,Test::e
//const Test::MAX_NUM_EXT=10

var t=new Test(10)*/

//System::println(Test::MAX_NUM_EXT)

/*var b=0
function(a){
	System::println(a)
	function(a){
		System::println(b)
	}(2)
}(1)*/


/*
function fun(a,b=-1){
	
}

fun(1);*/

/*function fun(a,b={a:0,b:1,c:2}){
	System::println(b)
	return 0
}*/

/*var a=0;

System::println(function(a){
	var r=0;
	System::println(r+a)
	
	return 9
}(1)+2)*/

//b();

/*function fun(a,b){
	
}*/

/*class A{
	var a
	A(a){
		function(a){
			System::println(a);			
		}		
		
		//b();
	}
	
}*/


//a=new A();
//a.f();

/*class A{
	A(){
		0
	}
	
	print(){
		System::println("mierda");
	}
}*/

/*var a=fun()

System::println(a)*/

/*var a=1,b,c

a+=b=c=a,1

System::println("{0} {1} {2}",a,b,c)*/

/*for(var i=0; i < 10; i++){
	
}*/


/*var a
a+=0*/
/*
class A{
	//var a
	A(){
		//this.a=0
		//this.b=1
	}

	fun(){
		System::println("sum="+(this.b))		
	}
	
}

var a=new A()
a.b=1
a.fun()*/

/*class A{}

function @default1(){
	return 0+1
}

function fun(a=0+1){
	var a=new A();
}

var a=undefined;

fun(a)

System::println(a)
*/




/*function fun1(){
	return 0,1;
}

function fun2(){
	return 2,3;
}

function fun3(){
	return 4,5,6;
}

function fun4(){
	return 7,8;
}

function fun5(){
	return 9,10,11;
}

var a,b,c,d,e,f,g,h,k,l,m,n,aux=false

a,b,c,d,e,f,g,h,k,l,m,n=fun1(),fun2(),fun3(),aux==false?fun4():fun5()

System::println("{0} {1} {2} {3} {4} {5} {6} {7} {8} {9} {10} {11}",a,b,c,d,e,f,g,h,k,l,m,n)*/

//var t=v[fun()];
//var g=false?1+fun():fun()

//var a=1;
//System::println("{0}",Math::randm())

//var b="vaca"
//const b=0;
/*class B{
	print(){System::println("dadsa")}	
}

class A{
	//const c=0
	var b="vaca",c=new B()//,a=0//,A::h=undefined
}*/

//var A::d=false
//const A::e=-1

//var a=new A();
/*var b=new B();

b.print()*/

/*var d=*//*a.c.print()*/

//d.print()

//System::println("{0} {1} {2}",a.a,b.b,b.c)

/*for(var i=0,j=0; i < 10 && true; i++, i++,0,j+=2,2,3,4){
	System::println("{0} {1}",i,j)
}


{
	//var c=1;

	//System::println("{0} {1} {2}",c)
}*/

//var a=0,b,c;

//var a=0,b=0,c={};

//a,b,c=0,1,2



/*function fun(){return 0,undefined,2,undefined}

var a,b,c,d;

//0,1,2,3,4,5,6,7,8,9,10,11,12,13

//var (a,b,c=fun()),b=1 //<-- it should be not possible

a,b,c=1,2,3,d=4*/

/*for(var k,v in it){
	k,v=it()
	it++;
}*/



//a+=b-=c=2

//System::println("{0} {1} {2} {3}",a,b,c,d)



/*var start=System::clock();
System::println(start);*/

/*var i=0;
for(i=0; i < 10; i++){
	
}*/


//System::println(0);
//String::format();
/*function fun(){}

fun()
*/
//return System::getZetScript()
//var a=0
//0+0,0,0
//System::println(0)
/*function f(){return 1,2+2+5,3;}

0+f();*/

//1+1+4*10/
//true || (false && false)+1
//var a=1,(b,c=2,3);
//a+b;
/*var a=1,b=2;

(1+a)-b*/
/*
function fun(
		a,b,c
	){
	//var d,f;
	return a;
}


fun(1,0,0,0)+fun(2,7,7,7,7,7,7,7)
*/



//System::println(System::clock()+100);

//var a,b,c;

//return 1;

//a,b=0,0
//"dads"+1+true+0.48668054j

//return 0;