# ZetScript

## Simple Script engine for C++

### Introduction

<p>ZetScript is a programming language and comes with and API that allows bind your C++ code into script side.</p><p>A Hello World sample is shown below,</p>
							
<pre lang="c++">
#include "zetscript.h"

int main(){

	zetscript::ScriptEngine script_engine;

	script_engine.compileAndRun("Console::outln(\"Hello World from ZetScript!\")");

	return 0;
}
</pre>

### Language overview
					
#### ZetScript types

ZetScript has defined types as integers, numbers, booleans, arrays and objects.

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

	function1(_a){
		this.data1 =_a;
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

// ZetScript C++ interface function
void sayHelloWorld(zetscript::ScriptEngine *_script_engine){
	printf("Hello world\n");
}
int main(){
	zetscript::ScriptEngine script_engine;

	// Registers sayHelloWorld as 'sayHelloWorld' symbol name
	script_engine.registerFunction("sayHelloWorld",sayHelloWorld);

	// Evaluates a script where it calls 'sayHelloWorld' function
	script_engine.compileAndRun(
		"sayHelloWorld();"
	);
	return 0;
}
</pre>

#### Exposing C++ types to ZetScript

To expose C++ type to ZetScript is done by registering C++ type. To expose members functions or variables is done by by defining and registering a C function.
						
<pre lang="c++">
#include "zetscript.h"

class MyClass{
public:
	int value;

	MyClass(){
		this->value=0;
	}

	void setValue(int _value){
		this->value = _value;
	}

};

// define new and delete functions
MyClass *MyClassWrap_new(
	zetscript::ScriptEngine *_script_engine
){
	return new MyClass;
}

void MyClassWrap_delete(
	zetscript::ScriptEngine *_script_engine
	,MyClass *_this
){
	delete _this;
}

// bind data1 member variable (read & write)
void MyClassWrap_set_value(
	zetscript::ScriptEngine *_script_engine
	,MyClass *_this
	, zetscript::zs_int _value
){
	_this->value=_value;
}

zetscript::zs_int MyClassWrap_get_value(
	zetscript::ScriptEngine *_script_engine
	,MyClass *_this
){
	return _this->value;
}

// 'MyClassWrap::setData' wrap function
void MyClassWrap_setValue(
	zetscript::ScriptEngine *_script_engine
	,MyClass *_this
	, zetscript::zs_int _value
){
	_this->setValue(_value);
}

int main(){

	zetscript::ScriptEngine script_engine;

	// Register class type 'MyClass' as instantiable
	script_engine.registerType<MyClass>("MyClass",MyClassWrap_new,MyClassWrap_delete);

	// Register member variable 'MyClass::value' through get/set metamethod using 'MyClassWrap_set_value'/'MyClassWrap_get_value' functions
	script_engine.registerMemberPropertyMetamethod<MyClass>("value","_set",&MyClassWrap_set_value);
	script_engine.registerMemberPropertyMetamethod<MyClass>("value","_get",&MyClassWrap_get_value);

	// Register member function 'MyClass::setValue' using 'MyClassWrap_setValue' wrapper
	script_engine.registerMemberFunction<MyClass>("setValue",&MyClassWrap_setValue);

	// Compiles and runs script
	script_engine.compileAndRun(
		"var myclass = new MyClass();\n" // instances MyClassExtend from C++
		"myclass.setValue(5);\n" // it prints "Int value as 5"
		"Console::outln(\"myclass.value => {0}\",myclass.value);\n" // it prints "myclass.value => 5"
	);

	return 0;
}
</pre>

ZetScript allows inherits is that a script class can inherit c++ registered type.

<pre lang="c++">
#include "zetscript.h"

class MyClass{
public:
	int value;

	MyClass(){
		this->value=0;
	}

	void setValue(int _value){
		this->value = _value;
	}

};

// define new and delete functions
MyClass *MyClassWrap_new(
	zetscript::ScriptEngine *_script_engine
){
	return new MyClass;
}

void MyClassWrap_delete(
	zetscript::ScriptEngine *_script_engine
	,MyClass *_this
){
	delete _this;
}

// bind data1 member variable (read & write)
void MyClassWrap_set_value(
	zetscript::ScriptEngine *_script_engine
	,MyClass *_this
	, zetscript::zs_int _value
){
	_this->value=_value;
}

zetscript::zs_int MyClassWrap_get_value(
	zetscript::ScriptEngine *_script_engine
	,MyClass *_this
){
	return _this->value;
}

// 'MyClassWrap::setData' wrap function
void MyClassWrap_setValue(
	zetscript::ScriptEngine *_script_engine
	,MyClass *_this
	, zetscript::zs_int _value
){
	_this->setValue(_value);
}

int main(){

	zetscript::ScriptEngine script_engine;

	// Register class type 'MyClass' as instantiable
	script_engine.registerType<MyClass>("MyClass",MyClassWrap_new,MyClassWrap_delete);

	// Register member variable 'MyClass::value' through get/set metamethod using 'MyClassWrap_set_value'/'MyClassWrap_get_value' functions
	script_engine.registerMemberPropertyMetamethod<MyClass>("value","_set",&MyClassWrap_set_value);
	script_engine.registerMemberPropertyMetamethod<MyClass>("value","_get",&MyClassWrap_get_value);

	// Register member function 'MyClass::setValue' using 'MyClassWrap_setValue' wrapper
	script_engine.registerMemberFunction<MyClass>("setValue",&MyClassWrap_setValue);

	// Compiles and runs script
	script_engine.compileAndRun(
		"class ScriptMyClass extends MyClass{\n"
		"	setValue(_value){\n"
		"		super(_value*2);\n" // it calls MyClass::setValue from C++
		"		this.value=this.value+10;\n"
		"   }\n"
		"};\n"

		"var myclass = new ScriptMyClass();\n" // instances ScriptMyClass and MyClass from C++
		"myclass.setValue(5);\n"
		"Console::outln(\"myclass.value => {0}\",myclass.value);\n" // it prints "myclass.value => 20"
	);

	return 0;
}
</pre>

#### Call ZetScript from c++
Once you have evaluated the script you can call function script from c++.
						
<pre lang="c++">
	
#include "zetscript.h"

int main(){

	zetscript::ScriptEngine script_engine;

	script_engine.compile(
		"function sayHello(){\n"
		"	Console::outln(\"call from 'sayHello'!\")\n"
		"}\n"
	);

	// instance ZetScript function 'sayHello'
	auto  say_hello=script_engine.bindScriptFunction<void ()>("sayHello");

	// it calls 'say_hello' script function from C++
	say_hello();

	return 0;
}
   
</pre>					
