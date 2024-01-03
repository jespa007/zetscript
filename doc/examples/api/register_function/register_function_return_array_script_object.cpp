#include "zetscript.h"

// Definition of the native function interface returnArray
zetscript::ArrayScriptObject *returnArray(
	zetscript::ScriptEngine *_script_engine
){
	// instance new ArrayScriptObject using ScriptEngine instance
	zetscript::ArrayScriptObject *array=_script_engine->newArrayScriptObject();

	// push first value as integer 10
	array->push<zetscript::zs_int>(10);

	// push second value as float 5.5
	array->push<zetscript::zs_float>(5.5);

	// push third value as boolean true
	array->push<bool>(true);

	// push 4rth reference string "Hello Word"
	array->push<const char *>("Hello World");

	// return object array
    return array;
}

int main(){
	zetscript::ScriptEngine script_engine;

	// bind native function returnArray named as 'returnArray'
    script_engine.registerFunction("returnArray",returnArray);

    // Eval script that calls native function 'returnArray'
    script_engine.compileAndRun(
        "Console::outln(\"result : \"+returnArray());"
 	);

    return 0;
}


