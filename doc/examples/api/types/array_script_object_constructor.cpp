#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::ArrayScriptObject;

ArrayScriptObject *returnNewArrayScriptObject(ScriptEngine *_script_engine){

	// instance new StringScriptObject using ScriptEngine instance
	ArrayScriptObject *array_object=new ArrayScriptObject(_script_engine);

	//...

	return array_object;
}

int main(){
	zetscript::ScriptEngine script_engine;

	// Registers returnNewArrayScriptObject
	script_engine.registerFunction("returnNewArrayScriptObject",returnNewArrayScriptObject);

	// prints the contents of new array object (empty)
	script_engine.compileAndRun("Console::outln(returnNewArrayScriptObject())");
	
	return 0;
}