#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::ObjectScriptObject;

ObjectScriptObject *returnNewObject(ScriptEngine *_script_engine){

	// instance new StringScriptObject using ScriptEngine context
	ObjectScriptObject *new_object=new ObjectScriptObject(_script_engine);

	//...

	return new_object;
}

int main(){
	zetscript::ScriptEngine script_engine;

	// Registers returnNewObject
	script_engine.registerFunction("returnNewObject",returnNewObject);

	// prints the contents of new array object (empty)
	script_engine.eval("Console::outln(returnNewObject())");
	
	return 0;
}