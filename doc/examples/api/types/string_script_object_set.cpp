#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::StringScriptObject;

StringScriptObject *returnString(ScriptEngine *_script_engine){
	StringScriptObject *string_object = new StringScriptObject(_script_engine);
	string_object->set("Hello World!");
	return string_object;
}

int main(){
	zetscript::ScriptEngine script_engine;

	// registers returnString
	script_engine.registerFunction("returnString",returnString);

	// prints the value returned by returnString
	script_engine.compileAndRun("Console::outln(returnString())");
	
	return 0;
}