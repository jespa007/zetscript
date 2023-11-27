#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::ObjectObject;

ObjectObject *returnNewObject(ZetScript *_zs){

	// instance new StringObject using ZetScript context
	ObjectObject *new_object=new ObjectObject(_zs);

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