# ZetScript

## Simple Script engine for C++

### Introduction

<p>ZetScript is a programming language and comes with and API that allows bind your C++ code into script side.</p><p>A Hello World sample is shown below,</p>
							
<pre lang="c++">
#include "zetscript.h"

int main(){

	zetscript::ZetScript zs;

	zs.compileAndRun("Console::outln(\"Hello World from script!\")");

	return 0;
}
</pre>

### Language overview
					
#### ZetScript types

Zetscript has defined types as integers, numbers, booleans, arrays and objects.

<pre lang="javascript">
var i=10; //integer
var f=0.5; // float
var s="a string"; // string
var b=true; // boolean

var array=[1,0.5, "a string", true]; // array

var object={ // object
	i: 10,
	f: 0.5,
	s: "a string",
	b: true,
	v: [1,0.5, "a string", true]
};
 
</pre>
						
#### Conditionals

Zetscript supports if-else, switch conditionals and more
<pre lang="javascript">
// if-else conditional
if(number < 10){
	Console::outln("number less than 10");
}else{
	Console::outln("number greater equal than 10");
}

// switch conditional
switch(number){
case 0:
case 1: 
	Console::outln("number is 0 or 1");
	break;
case 2:
	Console::outln("number is 2");
	break;
default:
	Console::outln("number is : "+number);
	break;
}
</pre>

#### Loops</h3>

Zetscript supports while,do-while,for and for-in as loops iterators

<pre lang="javascript">
var i=0;
// while loop
while(i &lt; 10){
	Console::outln("i:"+i);
	i++;
}

// do-while loop
do{
	Console::outln("i:"+i);
	i++;
}while(i &lt; 20);

// for loop
for(var j=0; j < 10; j++){
	Console::outln("j:"+i);
}

// for-in loop
var array=[1,0.5, "a string", true]; // array

for(var v in array){
	Console::outln("value:"+v);
}

</pre>						

#### Classes and inheritance

Zetscript supports class and inheritance. Function and variables members are referenced through <b>this</b> keyword. Also it can define variables/functions later. Inheritance support <b>super()</b> function in order to call parent function. To instance class is done through <b>new</b> operator.
						

<pre lang="javascript">
// A class example
class Test{

	function1(a){
		this.data1 =a;
		Console::outln("calling from Test. Data1:"+this.data1);
	}
};

// include member variable data2
var Test::data2; 

// include member function function2
function Test::function2(){ 
	this.data2="a string";
}

// A inheritance class example. 
// TestExtended inherites data1,data2,function1 and function2. 
class TestExtended extends Test{
	function1(a){
		super(2); // it calls Test::function1(2)
		this.data1+=5; // Now data1=5+2 = 7
		Console::outln("calling from TestExtended. Data1:"+this.data1);
	}
	
	function3(){ // 
		this.data3=6;
		Console::outln("data3 is "+this.data3);
	}
};

var t=new TestExtended(); // instances TestExtended class
</pre>
						
### API Overview
				
#### Call C++ code from ZetScript

To call C++ code from ZetScript is done by defining and registering a C function. 
						
<pre lang="c++">
#include "zetscript.h"
using zetscript::ZetScript;
// ZetScript C++ interface function
void sayHelloWorld(ZetScript *_zs){
	printf("Hello world\n");
}
int main(){
	ZetScript zs;
	// Registers sayHelloWorld as 'sayHelloWorld' symbol name
	zs.registerFunction("sayHelloWorld",sayHelloWorld);
	// Evaluates a script where it calls 'sayHelloWorld' function
	zs.compileAndRun(
	"sayHelloWorld();"
	);
	return 0;
}
</pre>

#### Exposing C++ types to ZetScript

To expose C++ type to ZetScript is done by registering C++ type. To expose members functions or variables is done by by defining and registering a C function.
						
<pre lang="c++">
#include "ZetScript.h"

using zetscript::ZetScript;
using zetscript::zs_int;


class MyClass{
public:
	int data1;

	MyClass(){
		this->data1=0;
	}

	void setData(int _data){
		this->data1 = _data;
		printf("Int value as %i\n",this->data1);
	}
	
};

// define new and delete functions
MyClass *MyClassWrap_new(ZetScript *_zs){
	return new MyClass;
}

void MyClassWrap_delete(ZetScript *_zs,MyClass *_this){
	delete _this;
}

// bind data1 member variable (read & write)
void MyClassWrap_set_data1(ZetScript *_zs,MyClass *_this, zs_int v){
	_this->data1=v;
}

zs_int MyClassWrap_get_data1(ZetScript *_zs,MyClass *_this){
	return _this->data1;
}

// 'MyClassWrap::setData' wrap function
void MyClassWrap_setData(ZetScript *_zs,MyClass *_this, zs_int v){
	_this->setValue(v);
}

int main(){

	ZetScript zs;

	// Register class type 'MyClass' as instantiable
	zs.registerType<MyClass>("MyClass",MyClassWrap_new,MyClassWrap_delete);

	// Register member variable 'MyClass::data1' through get/set metamethod using 'MyClassWrap_set_data1'/'MyClassWrap_get_data1' functions
	zs.registerMemberPropertyMetamethod<MyClass>("_set","data1",&MyClassWrap_set_data1);
	zs.registerMemberPropertyMetamethod<MyClass>("_get","data1",&MyClassWrap_get_data1);

	// Register member function 'MyClass::setData' using 'MyClassWrap_setData' wrapper
	zs.registerMemberFunction<MyClass>("setData",&MyClassWrap_setData);
	zs.compileAndRun(
		"var myclass = new MyClass();\n" // instances MyClassExtend from C++
		"myclass.setData(5);\n" // it prints "Int value as 5"
		"Console::outln(\"data1:\"+myclass.data1);\n" // it prints "data1:5"
	);
	
	return 0;
}
	
</pre>
<br>
ZetScript allows inherits is that a script class can inherit c++ registered type.
<br>
<pre lang="c++">
int main(){

	// ...

	zs.compileAndRun(
		"class ScriptMyClass extends MyClass{\n"
		"	setData(arg){\n"
		"		var i=this.data1;\n"
		"		super(this.data1+arg);\n" // it calls MyClass::setData from C++ 
		"		Console::outln(\"result => \"+i+\"+\"+arg+\"=\"+this.data1);\n"
		"   }\n"
		"};\n"

		"var myclass = new ScriptMyClass();\n" // instances ScriptMyClass and MyClass from C++
		"myclass.setData(5);\n"
	);

	return 0;
}
</pre>
<br>
It prints
<br>
```
Int value as 10
result => 5+5=10
```

#### Call ZetScript from c++
Once you have evaluated the script you can call function script from c++.
						
<pre lang="c++">
	
#include "zetscript.h"

void test_call_script_function(){

	zetscript::ZetScript zs;

	zs.compileAndRun(
		"function sayHello(){\n"
		"	Console::outln(\"hello from 'sayHello'!\")\n"
		"}\n"
	);

	// instance ZetScript function 'sayHello'
	auto  say_hello=zs.bindFunction<void ()>("sayHello");

	// it calls 'say_hello' script function from C++
	say_hello();

	return 0;
}
   
</pre>					
