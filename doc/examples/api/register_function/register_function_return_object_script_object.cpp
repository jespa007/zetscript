#include "zetscript.h"

// Definition of the native function interface returnObject
zetscript::ObjectScriptObject *returnObject(
	zetscript::ScriptEngine *_script_engine
){
	// instance new ObjectScriptObject using ScriptEngine instance
	zetscript::ObjectScriptObject *object=_script_engine->newObjectScriptObject();

	// instance new ArrayScriptObject using ScriptEngine instance
	zetscript::ArrayScriptObject *array=_script_engine->newArrayScriptObject();

	// set field "a" as integer 10
	object->set<zetscript::zs_int>("a",10);

	// set field "b" as float 5.5
	object->set<zetscript::zs_float>("b",5.5);

	// set field "c" as boolean true
	object->set<bool>("c",true);

	// set field "d" as string
	object->set<const char *>("d","Hello World");

	// initialize vector object and set as "d"
	for(int i=0; i < 10; i++){
		array->push<zetscript::zs_int>(i);
	}

	object->set<zetscript::ArrayScriptObject *>("d",array);

	// return object
    return object;
}

int main(){
	zetscript::ScriptEngine script_engine;

	// bind native function returnObject named as 'returnObject'
    script_engine.registerFunction("returnObject",returnObject);

    // Eval script that calls native function 'returnObject'
    script_engine.compileAndRun(
        "Console::outln(\"result : \"+returnObject());"
 	);

    return 0;
}


