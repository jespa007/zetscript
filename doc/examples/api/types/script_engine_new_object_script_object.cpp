#include "zetscript.h"

zetscript::ObjectScriptObject *returnNewObject(
	zetscript::ScriptEngine *_script_engine
){

	// instance new StringScriptObject using ScriptEngine instance
	zetscript::ObjectScriptObject *new_object=_script_engine->newObjectScriptObject();


	// set a integer
 	new_object->set<zetscript::zs_int>("i",5);

	// set a float
	new_object->set<zetscript::zs_float>("f",10.2);

	// set string
	new_object->set<zetscript::StringScriptObject *>("s",new zetscript::StringScriptObject(_script_engine,"Hello"));

	// set an array 
	zetscript::ArrayScriptObject *new_array=_script_engine->newArrayScriptObject();
	for(int i=0; i < 3; i++){
		new_array->push<zetscript::zs_int>(i+1);
	}
	new_object->set<zetscript::ArrayScriptObject *>("a",new_array);

	// set an object 
	zetscript::ObjectScriptObject *new_object2=_script_engine->newObjectScriptObject();

	for(int i=0; i < 3; i++){
		new_object2->set<zetscript::zs_int>(zetscript::String::format("p%i",i+1),(i+1)*2);
	}

	new_object->set<zetscript::ObjectScriptObject *>("o",new_object2);


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