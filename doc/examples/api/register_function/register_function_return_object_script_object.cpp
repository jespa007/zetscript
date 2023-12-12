#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::zs_int;
using zetscript::zs_float;
using zetscript::String;
using zetscript::ArrayScriptObject;
using zetscript::ObjectScriptObject;

// Definition of the native function interface returnObject
ObjectScriptObject *returnObject(ScriptEngine *_se){
	// instance new ObjectScriptObject using ScriptEngine context
	ObjectScriptObject *object=_se->newObjectScriptObject();

	// instance new ArrayScriptObject using ScriptEngine context
	ArrayScriptObject *array=_se->newArrayScriptObject();

	// set field "a" as integer 10
	object->set<zs_int>("a",10);

	// set field "b" as float 5.5
	object->set<zs_float>("b",5.5);

	// set field "c" as boolean true
	object->set<bool>("c",true);

	// set field "d" as string
	object->set<const char *>("d","Hello World");

	// initialize vector object and set as "d"
	for(int i=0; i < 10; i++){
		array->push<zs_int>(i);
	}

	object->set<ArrayScriptObject *>("d",array);

	// return object
    return object;
}

int main(){
	ScriptEngine se;

	// bind native function returnObject named as 'returnObject'
    se.registerFunction("returnObject",returnObject);

    // Eval script that calls native function 'returnObject'
    se.compileAndRun(
        "Console::outln(\"result : \"+returnObject());"
 	);

    return 0;
}


