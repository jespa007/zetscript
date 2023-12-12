#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::StringScriptObject;

StringScriptObject *returnNewStringScriptObject(ScriptEngine *_zs){

	// instance new StringScriptObject using ScriptEngine context
	StringScriptObject *string_object=new StringScriptObject(_zs,"Hello World!");

	// ...

	return string_object;

}

int main(){
	zetscript::ScriptEngine zs;

	// Registers returnNewStringScriptObject
	zs.registerFunction("returnNewStringScriptObject",returnNewStringScriptObject);

	// prints the returning of new string object
	zs.compileAndRun("Console::outln(returnNewStringScriptObject())");
	
	return 0;
}