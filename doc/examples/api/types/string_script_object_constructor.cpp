#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::StringScriptObject;

StringScriptObject *returnNewStringScriptObject(ScriptEngine *_se){

	// instance new StringScriptObject using ScriptEngine context
	StringScriptObject *string_object=new StringScriptObject(_se,"Hello World!");

	// ...

	return string_object;

}

int main(){
	zetscript::ScriptEngine se;

	// Registers returnNewStringScriptObject
	se.registerFunction("returnNewStringScriptObject",returnNewStringScriptObject);

	// prints the returning of new string object
	se.compileAndRun("Console::outln(returnNewStringScriptObject())");
	
	return 0;
}