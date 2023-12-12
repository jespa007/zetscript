#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::StringScriptObject;

StringScriptObject *returnString(ScriptEngine *_se){
	StringScriptObject *string_object = new StringScriptObject(_se);
	string_object->set("Hello World!");
	return string_object;
}

int main(){
	zetscript::ScriptEngine se;

	// registers returnString
	se.registerFunction("returnString",returnString);

	// prints the value returned by returnString
	se.compileAndRun("Console::outln(returnString())");
	
	return 0;
}