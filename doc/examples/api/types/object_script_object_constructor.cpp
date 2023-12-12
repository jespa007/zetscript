#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::ObjectScriptObject;

ObjectScriptObject *returnNewObject(ScriptEngine *_se){

	// instance new StringScriptObject using ScriptEngine context
	ObjectScriptObject *new_object=new ObjectScriptObject(_se);

	//...

	return new_object;
}

int main(){
	zetscript::ScriptEngine se;

	// Registers returnNewObject
	se.registerFunction("returnNewObject",returnNewObject);

	// prints the contents of new array object (empty)
	se.compileAndRun("Console::outln(returnNewObject())");
	
	return 0;
}