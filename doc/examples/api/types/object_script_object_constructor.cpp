#include "zetscript.h"

zetscript::ObjectScriptObject *returnNewObject(
	zetscript::ScriptEngine *_script_engine
){
	// instance new StringScriptObject using ScriptEngine instance
	zetscript::ObjectScriptObject *new_object=new zetscript::ObjectScriptObject(_script_engine);

	//...

	return new_object;
}

int main(){
	zetscript::ScriptEngine script_engine;

	// Registers returnNewObject
	script_engine.registerFunction("returnNewObject",returnNewObject);

	// prints the contents of new array object (empty)
	script_engine.compileAndRun("Console::outln(returnNewObject())");
	
	return 0;
}