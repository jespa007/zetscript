#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::ArrayScriptObject;

ArrayScriptObject *returnNewArrayScriptObject(ZetScript *_zs){

	// instance new StringScriptObject using ZetScript context
	ArrayScriptObject *array_object=new ArrayScriptObject(_zs);

	//...

	return array_object;
}

int main(){
	zetscript::ZetScript zs;

	// Registers returnNewArrayScriptObject
	zs.registerFunction("returnNewArrayScriptObject",returnNewArrayScriptObject);

	// prints the contents of new array object (empty)
	zs.compileAndRun("Console::outln(returnNewArrayScriptObject())");
	
	return 0;
}