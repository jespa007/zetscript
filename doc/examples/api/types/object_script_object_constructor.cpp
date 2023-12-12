#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::ObjectScriptObject;

ObjectScriptObject *returnNewObject(ScriptEngine *_zs){

	// instance new StringScriptObject using ScriptEngine context
	ObjectScriptObject *new_object=new ObjectScriptObject(_zs);

	//...

	return new_object;
}

int main(){
	zetscript::ScriptEngine zs;

	// Registers returnNewObject
	zs.registerFunction("returnNewObject",returnNewObject);

	// prints the contents of new array object (empty)
	zs.compileAndRun("Console::outln(returnNewObject())");
	
	return 0;
}