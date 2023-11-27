#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::StringObject;

StringObject *returnNewStringObject(ZetScript *_zs){

	// instance new StringObject using ZetScript context
	StringObject *string_object=new StringObject(_zs,"Hello World!");

	// ...

	return string_object;

}

int main(){
	zetscript::ZetScript zs;

	// Registers returnNewStringObject
	zs.registerFunction("returnNewStringObject",returnNewStringObject);

	// prints the returning of new string object
	zs.eval("Console::outln(returnNewStringObject())");
	
	return 0;
}