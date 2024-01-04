#include "zetscript.h"

zetscript::StringScriptObject *returnString(
	zetscript::ScriptEngine *_script_engine
){
	zetscript::StringScriptObject *string_object = new zetscript::StringScriptObject(_script_engine);
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