#include "zetscript.h"

using zetscript::zs_int;
using zetscript::zs_float;
using zetscript::ScriptEngine;
using zetscript::String;
using zetscript::StringScriptObject;
using zetscript::ArrayScriptObject;
using zetscript::ObjectScriptObject;

ObjectScriptObject *returnNewObject(ScriptEngine *_script_engine){

	// instance new StringScriptObject using ScriptEngine instance
	ObjectScriptObject *new_object=_script_engine->newObjectScriptObject();


	// set a integer
 	new_object->set<zs_int>("i",5);

	// set a float
	new_object->set<zs_float>("f",10.2);

	// set string
	new_object->set<StringScriptObject *>("s",new StringScriptObject(_script_engine,"Hello"));

	// set an array 
	ArrayScriptObject *new_array=_script_engine->newArrayScriptObject();
	for(int i=0; i < 3; i++){
		new_array->push<zs_int>(i+1);
	}
	new_object->set<ArrayScriptObject *>("a",new_array);

	// set an object 
	ObjectScriptObject *new_object2=_script_engine->newObjectScriptObject();

	for(int i=0; i < 3; i++){
		new_object2->set<zs_int>(String::format("p%i",i+1),(i+1)*2);
	}

	new_object->set<ObjectScriptObject *>("o",new_object2);


	return new_object;
}

int main(){
	zetscript::ScriptEngine script_engine;

	// Registers returnNewObject
	script_engine.registerFunction("returnNewObject",returnNewObject);

	// prints the contents of new object
	script_engine.compileAndRun("Console::outln(returnNewObject())");
	
	return 0;
}