#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::zs_int;
using zetscript::zs_float;
using zetscript::ArrayScriptObject;
using zetscript::ObjectScriptObject;

ArrayScriptObject *returnNewArray(ScriptEngine *_script_engine){

	// instance new ArrayScriptObject using ScriptEngine instance
	ArrayScriptObject *array_object=_script_engine->newArrayScriptObject();

	// push first value as integer 10
	array_object->push<zs_int>(10);

	// push second value as float 5.5
	array_object->push<zs_float>(5.5);

	// push third value as boolean true
	array_object->push<bool>(true);

	// push 4rth reference string "Hello Word"
	array_object->push<const char *>("Hello World");

	// push 5th value as an array 
	ArrayScriptObject *new_array=_script_engine->newArrayScriptObject();
	for(int i=0; i < 3; i++){
		new_array->push<zs_int>(i+1);
	}
	array_object->push<ArrayScriptObject *>(new_array);

	// push 6th value as an object 
	ObjectScriptObject *new_object2=_script_engine->newObjectScriptObject();

	for(int i=0; i < 3; i++){
		new_object2->set<zs_int>(String::format("p%i",i+1),(i+1)*2);
	}

	array_object->push<ObjectScriptObject *>(new_object2);


	return array_object;
}

int main(){
	zetscript::ScriptEngine script_engine;

	// registers returnNewArray
	script_engine.registerFunction("returnNewArray",returnNewArray);

	// prints the contents returned by returnNewArray
	script_engine.compileAndRun("Console::outln(returnNewArray())");
	
	return 0;
}
