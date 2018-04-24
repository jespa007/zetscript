ZetScript Programming Language

jespa007, 22 Apr 2018
 
	   4.64 (26 votes)
Rate:	
vote 1vote 2vote 3vote 4vote 5
A programming language for C++ projects
Download zetscript-1.3.0-src.zip - 160.1 KB
Download zetscript-1.3.0-win32-x86.zip - 150.7 KB
You can also run an interactive console here.

Introduction
ZetScript is a programming language with an API that allows bind C++ code in script side. ZetScript includes the following features:

Virtual Machine
Language syntax close to Javascript
MSVC++ 32/64 bits MSVC 2015/2017 or build tools v141
Linux/MinGW 32/64 bits, g++ 4.8 or above
Save/Load state support
Dynamic Garbage collector
Straightforward way to bind C++ variables, functions, classes and its members
The library size is 1Mb on gnu toolchain and 372KB on MSVC++
Override operators through metamethods
A helloworld in shown in the following code,

Hide   Copy Code
#include "CZetScript.h"

using namespace zetscript;

void say_helloworld(){
	printf("Hello World!");
}

int main(){

	CZetScript *zs = CZetScript::getInstance(); // instance zetscript

	register_C_Function("say_helloworld",say_helloworld);

	zs->eval("say_helloworld();"); // Call c function and prints hello world!
	return 0;
}
List 1.1

The code presented on list 1.1 it registers C function say_helloworld with just one line of code. Then, it calls eval function that evaluates the string say_helloworld(); that means execute a call of C funtion say_helloword from the script engine.

Install
From the source code included in this article, the install is quite easy, it uses cmake method so to configure the project depending which compiler is installed,

Hide   Copy Code
cmake CMakeLists.txt
 

Quote:
Note: MSVC 2017, it has a feature a Open Folder that easily configures a CMakeFile project just opening the folder where the project is, avoiding configure the project in the same directory and leave a lot of files and directories related with configuration


Yet another script language ?
Few years ago, I decided to start work with scripting because I needed to be faster in terms of production. Of course, because I always try to be a good engineer I started to play with all script engines that already are in this world. 

Some of them were faster but it I didn't like its syntax. Others, the sintaxis was good but slow at run-time, etc. After spent a lot of time playing with them and trying to decide what's kind of script engine could be a better candidate for my project I finished to know that none of them fitted at all for my neededs. 

So two years ago decided to start my own script engine, but apart of putting my effort with my neededs I read from some forums what it expects to have from a good scrip engine, I mixed the both things together and It became ZetScript.

At the end I decided to put ZetScript free because I like make easy life for the people, just in case they find useful this tool.

Language overview
Built-in types
Zetscript has a built in basic types like integers, numbers, booleans and containers as vectors and structures.

Hide   Copy Code
var i=10; //integer
var f=0.5; // number
var s="a string"; // string
var b=true; // boolean

var vector=[1,0.5, "a string", true]; // vector

var struct={ // structure
	i: 10,
	f: 0.5,
	s: "a string",
	b: true,
	v: [1,0.5, "a string", true]
};
 

Scope
ZetScript has an easy concept of global scope declaring variables in the top of evaluating scope. Local variables are within blocks like function or loops. Local vars are destroyed when it exits from block, unless is not referenced by other variable

Hide   Copy Code
var i= 0; // global var (never is destroyed when is declared)

{ // starting block --> declaring local variables starts here. 
  // You can access also to global var.

	var j= 2; // local var 
	// ...

} // ending block --> j is destroyed
Conditionals
Zetscript supports if-else and switch conditionals

Hide   Copy Code
// if-else conditional
var number=5;
if(number < 10){
	print("number less than 10");
}else{
	print("number greater equal than 10");
}

// switch conditional
switch(number){
case 0:
case 1: 
	print("number is 0 or 1");
	break;
case 2:
	print("number is 2");
	break;
default:
	print("number is : "+number);
	break;
}
Loops
Zetscript supports while,do-while and for as loops iterators

Hide   Copy Code
var i=0;
// while loop
while(i < 10){
	print("i:"+i);
	i++;
}

// do-while loop
do{
	print("i:"+i);
	i++;
}while(i < 20);

// for loop
for(var j=0; j < 10; j++){
	print("j:"+i);
}
Classes and inheritance
Zetscript supports class and inheritance. member and function variables inside class scope are referenced through this keyword. Also it can include variables/functions in post class declaration through operator :: . Inheritance supports super() function in order to call parent function. To instance class is done through new operator.

Hide   Shrink    Copy Code
// A class example
class Test{
	var data1;
	
	function function1(a){
		this.data1 =a;
		print("calling from Test. Data1:"+this.data1);
	}
};

// include member variable data2
var Test::data2; 

// include function function function2
function Test::function2(){ 
	this.data2="a string";
}

// A inheritance class example. 
// TestExtended inherites data1,data2,function1 and function2. 
class TestExtended: Test{
	var data3;
	function function1(a){
		super(2); // it calls Test::function1(2)
		this.data1+=5; // Now data1=5+2 = 7
		print("calling from TestExtended. Data1:"+this.data1);
	}
	
	function function3(){ // 
		this.data3=6;
		print("data3 is "+this.data3);
	}
};

var t=new TestExtended(); // instances TestExtended class
 

Calling script function from c++
Once you have evaluated the script you can call function script from c++ until up 6 parameters. To call script function from c++ it can be done through the function CZetScript::bind_function. Just pass the name of the function with the c++ template casting and it creates an std::function that links the script function. Also it is possible to link/call function member from instancied object on global scope.

Hide   Shrink    Copy Code
#include "CZetScript.h"

using namespace zetscript;

int main(){

	CZetScript *zs = CZetScript::getInstance(); // instance zetscript

	zs->eval(
		"class Test{"
		"	var data1;"
		"	function function1(arg){"
		"		print(\"calling Test.Function:\"+arg);"
		"	}"
		"};"
		""
		"function delete_test(){"
		"	delete test;"
		"	print(\"test variable was deleted\");"
		"}"
		""
		"var test=new Test();"
	);

	
    // instance function delete_test function.
	std::function<void()>  * delete_test=bind_function<void ()>("delete_test"); 

    // instance member function test.function1.
	std::function<void(int)> * test_function1=bind_function<void (int)>("test.function1"); 
	
    // it calls "test.function" member function with 10 as parameter.
	(*test_function1)(10); 

    // it calls "delete_test" function with no parameters
	(*delete_test)(); 

	// delete functions when they are used anymore
	delete 	test_function1;
	delete 	delete_test;

}
 

API overview
Bind C++ variables
Zetscrip can bind basic C types as int, float, bool and string types to operate in the script side.

Hide   Copy Code
#include "CZetScript.h"

using namespace zetscript;

int main(){

    int i=10;
	string	string_var = "in c++";
    bool b=false;
    float f=5.0;

	CZetScript *zs = CZetScript::getInstance(); // instance zetscript

    register_C_Variable("i",i); // it registers int variable called i
    register_C_Variable("b",b); // it registers bool var ble called b
    register_C_Variable("f",f); // it registers float variable called f
	register_C_Variable("string_var",string_var); // it registers string variable called string_var

	zs->eval(
        "i+=10;" // i+=10 => i=20
        "b=!b;" //  b=!b  => b=true
        "f+=10.5;" // f+=10.5 => f = 15.5
		"string_var+=\" and in script\";" // concatenates " and in script
		"print(\"string_var:\"+string_var);" // prints "string_var:in c++ and in script
	);
	return 0;
}
Bind C++ classes and its members
Binding C++ class in Zetscript is done easyly with register_C_Class method. To bind variables and functions members it can be done through register_C_VariableMember and register_C_FunctionMember respectively. In script you can instance the C++ class in script side through operator new. When the instanced C Class variable is not used anymore the user has to delete it with operator delete.

Hide   Shrink    Copy Code
#include "CZetScript.h"

using namespace zetscript;

class MyClass{
public:
	int data1;

	void function1(int arg){
		this->data1 = arg;
		printf("Int argument is %i\n",this->data1);
	}
};

class MyClassExtend:public MyClass{
public:
	float data2;

	void function2(float * arg){
		this->data2 = *arg;
		printf("Float argument is %.02f\n",this->data2);
	}
};

int main(){

	CZetScript *zs = CZetScript::getInstance(); // instance zetscript
	
    // register MyClass with name MyClass in script side.
	register_C_Class<MyClass>("MyClass"); 

    // register MyClassExtend with name MyClassExtend in script side.
	register_C_Class<MyClassExtend>("MyClassExtend"); 
	
    // tell that MyClassExtend is base of MyClass
	class_C_baseof<MyClassExtend,MyClass>(); 

    // register data1 named data1 in script side as variable member.
	register_C_VariableMember<MyClassExtend>("data1",&MyClass::data1); 

    // register function1 named function1 in script side as function member.
	register_C_FunctionMember<MyClassExtend>("function1",&MyClass::function1); 

    // register data2 named data1 in script side as variable member.
	register_C_VariableMember<MyClassExtend>("data2",&MyClassExtend::data2); 

    // register function2 named function2 in script side as function member.
	register_C_FunctionMember<MyClassExtend>("function2",&MyClassExtend::function2); 



	zs->eval(
		"var myclass = new MyClassExtend();" // instances MyClassExtend
		"myclass.function1(12);" // it prints "Int argument is 12"
		"myclass.function2(0.5);" // it prints "Float argument is 0.5"
		"print(\"data1:\"+myclass.data1);" // it prints "data1:12"
		"print(\"data2:\"+myclass.data2);" // it prints "data2:0.5"
		"delete myclass;" // delete script var with c pointers attached inside.
	);

	return 0;
}
List 1.2

Inheritance c++ Class on Script Class
An important feature of ZetScript is that it has supports c++ class inheritance for script class. this and super() keywords works as a normal behavior.

From list 1.2 we present an example script that ScripMyClassExtends class is inherited by MyClassExtends class (from C++),

Hide   Copy Code
class ScriptMyClassExtended: MyClassExtend{ // <-- inheritances MyClassExtend (c++)
  function function1(arg1){
    print("script argument is "+arg1)
    super(this.data1+arg1); // <-- calls MyClassExtend::function1 (c++) from list 1.2
  }
}

var myclass=new ScriptMyClassExtend();
Myclass.function1(5);
List 1.3

The code shown at list 1.3 will print,

Hide   Copy Code
script argument is 5
c++ argument is 15 
 

Metamethods
ZetScript implements metamethods to map operators or other operations through objects. Currently, it supports the following metamethods:

_equ (aka ==)
_not_equ (aka !=)
_lt (aka <)
_lte (aka >=)
_gt (aka >)
_gte (aka >=)
_not (aka !)
_neg (aka -)
_add (aka +)
_div (aka /)
_mul (aka *)
_mod (aka %)
_and (aka &)
_or (aka |)
_xor (aka ^)
_shl (aka <<)
_shr (aka >>) 
_set (aka =)
 

For example, if in script side we want to do the operation + for an object we have to declare the function _add with two parameters, as we can see in the following code,

Hide   Copy Code
class MyNumber{
  var num;
  function MyNumber(_n){
    this.num=_n;
  }
  function _add(op1,op2){
    return new MyNumber(op1.num+op2.num);
  }
};

var n1 = new MyNumber (20);
var n2 = new MyNumber (10);
var n3 =n1+n2;

print("n1 ("+n1.num+") n2 ("+n2.num+") = "+n3.num);
List 1.4

the same can be applied for C++ class. We have to register _add function in the C++ object,

Hide   Shrink    Copy Code
#include "CZetScript.h"

using namespace zetscript;

class MyNumber{
public:
    int num;
    MyNumber(){
        this->num=0;
    }
    MyNumber(int _n){
        this->num=_n;
    }
    void set(int _n){
        this->num=_n;
    }
    static MyNumber * _add(MyNumber *op1, MyNumber *op2){
        return new MyNumber(op1->num + op2->num);
    }
};
int main(){
    CZetScript *zs = CZetScript::getInstance();

    // register class MyNumber
    register_C_Class<MyNumber>("MyNumber");

    // register variable member num
    register_C_VariableMember<MyNumber>("num",&MyNumber::num);

    // register constructor through function MyNumber::set
    register_C_FunctionMember<MyNumber>("MyNumber",&MyNumber:: set);

    // register static function _add as metamethod
    register_C_StaticFunctionMember<MyNumber>("_add",&MyNumber::_add);
    
    if(!zs->eval(
        "var n1 = new MyNumber (20);\n"
        "var n2 = new MyNumber (10); \n"
        "var n3 =n1+n2; \n "
        "print(\"n1 (\"+n1.num+\") + n2 (\"+n2.num+\") = \"+n3.num);\n"
    )){
        fprintf(stderr,ZS_GET_ERROR_MSG());
    }
    return 0;
}
List 1.5

 

Quote:
Note that the codes shown at list 1.4 and 1.5 the _add function is static due operates from two objects and it doesn't the object member itself.

Save/Restore state
Sometimes is useful to reset the script cleaning global variables or restore from one point when, for example, C++ application is restarted.

ZetScript supports a way to save current compiled state. This operation saves AST nodes, registered C functions, variables and classes.

To save current state we have to invoke CState::saveState. This function returns an index that tells compiled state index saved.

Hide   Copy Code
int idx=CState::saveState()
To restore a previous state we have to invoke CState::restoreState passing compiled state index.

Hide   Copy Code
CState::setState(idx)
 

Performance
I had checked ZetScript performance 1.3.0 version with other script languages. Because Lua is the fastest and one of the most scripting language used I chose this to compare Fibonacci calculation time with N=34.

A Fibonacci script implemented in ZetScript is,

Hide   Copy Code
function fibR(n)
{
    if (n < 2) { 
         return n; 
    }

    return fibR(n-2)+fibR(n-1);
}

print("fib: " + (fibR(34)) );
List 1.6

and the equivalent in Lua is,

Hide   Copy Code
function fibR(n)

    if (n < 2) then return n end
    return (fibR(n-2) + fibR(n-1))
end

print("fib: " .. fibR(34))
List 1.7

 

And I have compared them through the time command already available in linux and the tests were made on a computer with i5-2450 CPU with 2.50GHz and 8GB of RAM.

The result was the following,

Lua: 1355ms
ZetScript: 3110ms
So It can say that Lua is 3110/1355=2,29 times faster than ZetScript in this test.









