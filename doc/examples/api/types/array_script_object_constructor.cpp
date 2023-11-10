#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::ArrayScriptObject;

ArrayScriptObject *returnNewArrayObject(ZetScript *_zs){

	// instance new StringScriptObject using ZetScript context
	ArrayScriptObject *array_object=new ArrayScriptObject(_zs);

	//...

	return array_object;
}

int main(){
	zetscript::ZetScript zs;

	// Registers returnNewArrayObject
	zs.registerFunction("returnNewArrayObject",returnNewArrayObject);

	// prints the contents of new array object (empty)
	zs.eval("Console::outln(returnNewArrayObject())");
	
	return 0;
}