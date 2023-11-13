#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::ObjectScriptObject;

ObjectScriptObject *returnNewObject(ZetScript *_zs){

	// instance new StringScriptObject using ZetScript context
	ObjectScriptObject *new_object=new ObjectScriptObject(_zs);

	//...

	return new_object;
}

int main(){
	zetscript::ZetScript zs;

	// Registers returnNewObject
	zs.registerFunction("returnNewObject",returnNewObject);

	// prints the contents of new array object (empty)
	zs.eval("Console::outln(returnNewObject())");
	
	return 0;
}