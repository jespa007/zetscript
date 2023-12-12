#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::ArrayScriptObject;
using zetscript::zs_int;
using zetscript::zs_float;
using zetscript::String;


// Definition of the native function interface returnArray
ArrayScriptObject *returnArray(ScriptEngine *_se){

	// instance new ArrayScriptObject using ScriptEngine context
	ArrayScriptObject *array=_se->newArrayScriptObject();

	// push first value as integer 10
	array->push<zs_int>(10);

	// push second value as float 5.5
	array->push<zs_float>(5.5);

	// push third value as boolean true
	array->push<bool>(true);

	// push 4rth reference string "Hello Word"
	array->push<const char *>("Hello World");

	// return object array
    return array;
}

int main(){
	ScriptEngine se;

	// bind native function returnArray named as 'returnArray'
    se.registerFunction("returnArray",returnArray);

    // Eval script that calls native function 'returnArray'
    se.compileAndRun(
        "Console::outln(\"result : \"+returnArray());"
 	);

    return 0;
}


