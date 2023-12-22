#include "zetscript.h"

using zetscript::ScriptEngine;

// Defines  MyType
class MyType{
//...
};

// defines global variable my_type
MyType *my_type=NULL;

MyType *returnMyType(ScriptEngine *_script_engine){
	return my_type;
}

int main(){
	zetscript::ScriptEngine script_engine;

	// Instances MyType
	my_type=new MyType();

	// Registers type 'MyType' as not instantiable
	script_engine.registerType<MyType>("MyType");

	// registers returnMyType function
	script_engine.registerFunction("returnMyType",returnMyType);

	// Gets an instance of 'MyType' and stores in 't'
	// Instances an object of type 'MyType' and stores in 't'
	script_engine.compileAndRun(
		"var t=returnMyType()"
		"Console::outln(\"typeof t =>\"+(typeof t));"
	);

	delete my_type;

	return 0;
}