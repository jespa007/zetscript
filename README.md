# ZetScript

## Simple Script engine for C++

### Introduction

<p>ZetScript is a programming language and comes with and API that allows bind your C++ code into script side.</p><p>A Hello World sample is shown below,</p>
							
```cpp
#include "zetscript.h"

int main(){

	zetscript::ScriptEngine script_engine;

	script_engine.compileAndRun("Console::outln(\"Hello World from ZetScript!\")");

	return 0;
}
```

ZetScript has the following features:

- Virtual Machine
- Script Language close to Javascript
- MSVC++ 32/64 bits MSVC 2015/2017/2019
- MacOS/Linux/MinGW 32/64 bits, g++ 4.8 or above
- Dynamic Garbage collector
- Straightforward way to expose C++ code in ZetScript and bind ZetScript code to C++ 
- Implement operators on types and member properties through metamethods

### Building ZetScript

The compilation and execution of ZetScript has been tested in the following plataforms:

- Linux through gcc toolchain
- MacOS through clang toolchain
- Windows through Mingw or VisualStudio 2015,2017/2019
- Raspberry PI through gcc toolchain

The building information is configured with cmake through the following command,

`cmake -Bbuild`

In Linux, MacOS and MingW environments compile the project with the following command,

`make -C build`

After the build, ZetScript llibrary and the command line tool will be placed at bin directory.

### Language overview
					
#### ZetScript types

ZetScript has defined types as integers, numbers, booleans, arrays and objects.

```javascript
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
 
```
						
#### Conditionals

Zetscript supports if-else, switch conditionals and more
```javascript
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
```

#### Loops</h3>

Zetscript supports while,do-while,for and for-in as loops iterators

```javascript
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

```						

#### Classes and inheritance

Zetscript it defines custom class with inheritance like javascript.
						

```javascript
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
	
	// Member property Entity::id
	id{
		// Member property metamethod _get only for read
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
```
						
### API Overview
				
#### Call C++ code from ZetScript

To call C++ code from ZetScript is done by defining and registering a C function. 
						
```cpp
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
```

#### Call ZetScript from c++
To call ZetScript from C++ code is done by binding script function after compile script function.
						
```cpp
	
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
```		

#### Exposing C++ types to ZetScript

To expose C++ type to ZetScript is done by registering C++ type. To expose members functions or variables is done by defining and registering a C function. In the following example shows and example of registering class `Entity`,
						
```cpp
#include "zetscript.h"

int n_entity=1;

// Class to register
class Entity{
public:
	const static int MAX_ENTITIES=10;

	int __id__=0;
	int health=0;
	std::string name="entity";
};

//-----------------------
// REGISTER FUNCTIONS

// Register function that returns a new instance of native 'Entity'
Entity *Entity_new(
	zetscript::ScriptEngine *_script_engine
){
	Entity *entity=new Entity;
	entity->__id__=n_entity++;
	entity->name="entity_"+std::to_string(entity->__id__);

	return entity;
}

// Register function that deletes native instance of 'Entity'
void Entity_delete(
	zetscript::ScriptEngine *_script_engine
	,Entity *_this
){
	delete _this;
}

// Register function that implements Entity::constructor
void Entity_constructor(
	zetscript::ScriptEngine *_script_engine
	,Entity *_entity
	,zetscript::String *_name
	,zetscript::zs_int _health
){
	_entity->name=_name->toConstChar();
	_entity->health=_health;
}

// Register function that implements const member variable Entity::MAX_ENTITIES
zetscript::zs_int Entity_MAX_ENTITIES(
	zetscript::ScriptEngine *_script_engine
){
	return Entity::MAX_ENTITIES;
}

// Register function that implements static member function Entity::isEntityDead
bool Entity_isEntityDead(
	zetscript::ScriptEngine *_script_engine
	,Entity *_entity
){
	return _entity->health==0;
}

// Register function that implements static member function metamethod Entity::_equ (aka ==)
bool Entity_nequ(
	zetscript::ScriptEngine *_script_engine
	,Entity *_e1
	,Entity *_e2
){
	return _e1->__id__!=_e2->__id__;
}

// Register function that implements member function metamethod Entity::update
void Entity_update(
	zetscript::ScriptEngine *_script_engine
	,Entity *_this
){
	printf("Update from Entity\n");
}


// Register function that implements member property id metamethod getter
zetscript::zs_int Entity_id_get(
	zetscript::ScriptEngine *_script_engine
	,Entity *_this
){
	return _this->__id__;
}

// Register function that implements member property name metamethod getter
zetscript::String Entity_name_get(
	zetscript::ScriptEngine *_script_engine
	,Entity *_this
){
	return _this->name.c_str();
}


// REGISTER FUNCTIONS
//-----------------------

int main(){

	zetscript::ScriptEngine script_engine;

	// Register type Entity
	script_engine.registerType<Entity>("Entity",Entity_new,Entity_delete);

	// Register Entity constructor
	script_engine.registerConstructor<Entity>(Entity_constructor);

	// Register constant member variable Entity::MAX_ENTITIES
	script_engine.registerConstMemberProperty<Entity>("MAX_ENTITIES",Entity_MAX_ENTITIES);

	// Register static member function Entity::isEntityDead
	script_engine.registerStaticMemberFunction<Entity>("isEntityDead",Entity_isEntityDead);

	// Register member function metamethod Entity::_equ (aka !=)
	script_engine.registerStaticMemberFunction<Entity>("_nequ",Entity_nequ);

	// Register member function Entity::update
	script_engine.registerMemberFunction<Entity>("update",Entity_update);

	// Register member property id getter
	script_engine.registerMemberPropertyMetamethod<Entity>("id","_get",Entity_id_get);

	// Register member property name getter
	script_engine.registerMemberPropertyMetamethod<Entity>("name","_get",Entity_name_get);

	// Compiles and runs script
	script_engine.compileAndRun(
		"var e1=new Entity();\n"
		"var e2=new Entity();\n"
		"Console::outln(\"Entity::MAX_ENTITIES: {0}\",Entity::MAX_ENTITIES);\n"
		"Console::outln(\"e1.id: {0} e1.name: {1} \",e1.id,e1.name);\n"
		"Console::outln(\"e2.id: {0} e2.name: {1} \",e2.id,e2.name);\n"
		"if(e1!=e2){\n"
		"  Console::outln(\"'e1' and 'e2' are NOT equals\")\n"
		"}\n"
		"e1.update();\n"
	);

	return 0;
}

```

Finally another interesting feature is that ZetScript can extend script class from C registered class. In the following example replaces the previus script with the following one that extends `Player` from `Entity`,
    
```cpp

	// Compiles and runs script
	script_engine.compileAndRun(
		"// Extends player from registered Entity\n"
		"class Player extends Entity{\n"
		"	constructor(){\n"
		"		super(\"Player\",10);\n"
		"	}\n"
		"\n"
		"	// Override Entity::update\n"
		"	update(){\n"
		"		// Calls Entity::update\n"
		"		super();\n"
		"		Console::outln(\"From player\");\n"
		"	}\n"
		"}\n"
		"\n"
		"var p=new Player();\n"
		"var e=new Entity();\n"
		"\n"
		"Console::outln(\"Entity::MAX_ENTITIES: {0}\",Entity::MAX_ENTITIES)\n"
		"Console::outln(\"p.id: {0} p.name: {1} \",p.id,p.name)\n"
		"\n"
		"if(p!=e){\n"
		"  Console::outln(\"'p' and 'e' are NOT equals\")\n"
		"}\n"
		"p.update();\n"
		"\n"
	);
```