#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::ArrayObject;

ArrayObject *returnNewArrayObject(ZetScript *_zs){

	// instance new StringObject using ZetScript context
	ArrayObject *array_object=new ArrayObject(_zs);

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