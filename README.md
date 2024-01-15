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

### Building ZetScript

ZetScript is straightforward configured as Release mode with cmake with the following command,  

 cmake -Bbuild
 
If cmake was configurated for VisualStudio open with visual studio and compile from there, or in case there's `msbuild` in the path it can be build as Release with the following command,

 msbuild build\zetscript.sln /p:Configuration=Release

If cmake configuration detects mingw or linux system launch make with the following command,

 make -C build
 
After the build, ZetScript llibrary and ZetScript command line tool with be placed at bin/ 

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
var n_entity=1;

class Entity{
	// Member variable initialization (this.__id__)
	var __id__=0;
	
	// Member const variable (Acces by Entity::MAX_ENTITIES)
	const MAX_ENTITIES=10;
	
	// Static member function
	static entityDead(_entity){
		return _entity.health==0;
	}

	// Member function metamethod _equ to make Entity comparable by '=='
	static _nequ(_e1,_e2){
		return _e1.id!=_e2.id;
	}
	
	// constructor
	constructor(_name="unknown",_health=0){
		this.name=_name
                this.__id__=n_entity++;
                this.setHealth(_health);
	}

	// Member function Entity::update()
	update(){
		Console::outln("From Entity")
	}
	
	// A definition of property
	id{
		// Member property metamethod _get to to get the value of this.__id__
		_get(){
			return this.__id__;
		}
	}
}

class Player extends Entity{
	constructor(){
		super("Player",10);
	}

	// Override Entity::update
	update(){
		// Calls Entity::update
		super();
		Console::outln("From player")
	}
}

var p=new Player();
var e=new Entity();

Console::outln("Entity::MAX_ENTITIES: {0}",Entity::MAX_ENTITIES)
Console::outln("p.id: {0} p.name: {1} p.health: {2}",p.id,p.name,p.health)

p.update();

if(p!=e){
  Console::outln("'p' and 'e' are NOT equals")
}
</pre>
						
### API Overview
				
#### Call C++ code from ZetScript

To call C++ code from ZetScript is done by defining and registering a C function. 
						
<pre lang="c++">
#include "zetscript.h"

// C function to register
void sayHelloWorld(zetscript::ScriptEngine *_script_engine){
	printf("Hello world\n");
}

int main(){
	zetscript::ScriptEngine script_engine;

	// Registers 'sayHelloWorld' function
	script_engine.registerFunction("sayHelloWorld",sayHelloWorld);

	// Compiles and run a script where it calls 'sayHelloWorld' function
	script_engine.compileAndRun(
		"sayHelloWorld();"
	);
	return 0;
}
</pre>

#### Call ZetScript from c++
To call ZetScript from C++ code is done by binding script function after compile script function.
						
<pre lang="c++">
	
#include "zetscript.h"

int main(){

	zetscript::ScriptEngine script_engine;

	// Compiles script function 'sayHello'
	script_engine.compile(
		"function sayHello(){\n"
		"	Console::outln(\"call from 'sayHello'!\")\n"
		"}\n"
	);

	// binds script function 'sayHello'
	auto  say_hello=script_engine.bindScriptFunction<void ()>("sayHello");

	// it calls 'say_hello' script function from C++
	say_hello();

	return 0;
}
   
</pre>		

#### Exposing C++ types to ZetScript

To expose C++ type to ZetScript is done by registering C++ type. To expose members functions or variables is done by by defining and registering a C function.
						
<pre lang="c++">
#include "zetscript.h"

// Class to register
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

//-----------------------
// REGISTER FUNCTIONS

// C function to register that returns an instance of native new 'MyClass'
MyClass *MyClassWrap_new(
	zetscript::ScriptEngine *_script_engine
){
	return new MyClass;
}

// C function to register that deletes an instance of native new 'MyClass'
void MyClassWrap_delete(
	zetscript::ScriptEngine *_script_engine
	,MyClass *_this
){
	delete _this;
}

// C function to register as 'MyClass::setValue' member function
void MyClassWrap_setValue(
	zetscript::ScriptEngine *_script_engine
	,MyClass *_this
	, zetscript::zs_int _value
){
	_this->setValue(_value);
}

// C function to register that implements setter metamethod for property 'value'
void MyClassWrap_set_value(
	zetscript::ScriptEngine *_script_engine
	,MyClass *_this
	, zetscript::zs_int _value
){
	_this->value=_value;
}

// C function to register that implements getter for property 'value'
zetscript::zs_int MyClassWrap_get_value(
	zetscript::ScriptEngine *_script_engine
	,MyClass *_this
){
	return _this->value;
}

// REGISTER FUNCTIONS
//-----------------------

int main(){

	zetscript::ScriptEngine script_engine;

	// Register type 'MyClass'
	script_engine.registerType<MyClass>("MyClass",MyClassWrap_new,MyClassWrap_delete);

	// Register property 'MyClass::value' with set/get metamethods through 'MyClassWrap_set_value'/'MyClassWrap_get_value' C functions
	script_engine.registerMemberPropertyMetamethod<MyClass>("value","_set",&MyClassWrap_set_value);
	script_engine.registerMemberPropertyMetamethod<MyClass>("value","_get",&MyClassWrap_get_value);

	// Register member function 'MyClass::setValue' through 'MyClassWrap_setValue' C function
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
