#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::StringScriptObject;

StringScriptObject *returnNewStringScriptObject(ScriptEngine *_script_engine){

	// instance new StringScriptObject using ScriptEngine instance
	StringScriptObject *string_object=new StringScriptObject(_script_engine,"Hello World!");

	// ...

	return string_object;

}

int main(){
	zetscript::ScriptEngine script_engine;

	// Registers returnNewStringScriptObject
	script_engine.registerFunction("returnNewStringScriptObject",returnNewStringScriptObject);

	// prints the returning of new string object
	script_engine.compileAndRun("Console::outln(returnNewStringScriptObject())");
	
	return 0;
}