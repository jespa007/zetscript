#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::zs_int;
using zetscript::zs_float;
using zetscript::ArrayScriptObject;

ArrayScriptObject *returnNewArray(ScriptEngine *_se){

	// instance new ArrayScriptObject using ScriptEngine context
	ArrayScriptObject *array_object=new ArrayScriptObject(_se);

	// push first value as integer 10
	array_object->push<zs_int>(10);

	// push second value as float 5.5
	array_object->push<zs_float>(5.5);

	// push third value as boolean true
	array_object->push<bool>(true);

	// push 4rth reference string "Hello Word"
	array_object->push<const char *>("Hello World");

	// return object array
    return array_object;
}

int main(){
	zetscript::ScriptEngine se;

	// registers returnNewArray
	se.registerFunction("returnNewArray",returnNewArray);

	// prints the value returned by returnNewArray
	se.compileAndRun("Console::outln(returnNewArray())");
	
	return 0;
}