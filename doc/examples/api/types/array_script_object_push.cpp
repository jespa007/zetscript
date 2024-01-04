#include "zetscript.h"

zetscript::ArrayScriptObject *returnNewArray(
	zetscript::ScriptEngine *_script_engine
){
	// instance new ArrayScriptObject using ScriptEngine instance
	zetscript::ArrayScriptObject *array_object=new zetscript::ArrayScriptObject(_script_engine);

	// push first value as integer 10
	array_object->push<zetscript::zs_int>(10);

	// push second value as float 5.5
	array_object->push<zetscript::zs_float>(5.5);

	// push third value as boolean true
	array_object->push<bool>(true);

	// push 4rth reference string "Hello Word"
	array_object->push<const char *>("Hello World");

	// return object array
    return array_object;
}

int main(){
	zetscript::ScriptEngine script_engine;

	// registers returnNewArray
	script_engine.registerFunction("returnNewArray",returnNewArray);

	// prints the value returned by returnNewArray
	script_engine.compileAndRun("Console::outln(returnNewArray())");
	
	return 0;
}