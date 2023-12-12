#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::ArrayScriptObject;

ArrayScriptObject *returnNewArrayScriptObject(ScriptEngine *_se){

	// instance new StringScriptObject using ScriptEngine context
	ArrayScriptObject *array_object=new ArrayScriptObject(_se);

	//...

	return array_object;
}

int main(){
	zetscript::ScriptEngine se;

	// Registers returnNewArrayScriptObject
	se.registerFunction("returnNewArrayScriptObject",returnNewArrayScriptObject);

	// prints the contents of new array object (empty)
	se.compileAndRun("Console::outln(returnNewArrayScriptObject())");
	
	return 0;
}