#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::StringScriptObject;

StringScriptObject *returnString(ScriptEngine *_zs){
	StringScriptObject *string_object = new StringScriptObject(_zs);
	string_object->set("Hello World!");
	return string_object;
}

int main(){
	zetscript::ScriptEngine zs;

	// registers returnString
	zs.registerFunction("returnString",returnString);

	// prints the value returned by returnString
	zs.compileAndRun("Console::outln(returnString())");
	
	return 0;
}