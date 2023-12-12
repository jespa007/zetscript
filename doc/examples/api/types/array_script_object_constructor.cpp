#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::ArrayScriptObject;

ArrayScriptObject *returnNewArrayScriptObject(ScriptEngine *_zs){

	// instance new StringScriptObject using ScriptEngine context
	ArrayScriptObject *array_object=new ArrayScriptObject(_zs);

	//...

	return array_object;
}

int main(){
	zetscript::ScriptEngine zs;

	// Registers returnNewArrayScriptObject
	zs.registerFunction("returnNewArrayScriptObject",returnNewArrayScriptObject);

	// prints the contents of new array object (empty)
	zs.compileAndRun("Console::outln(returnNewArrayScriptObject())");
	
	return 0;
}