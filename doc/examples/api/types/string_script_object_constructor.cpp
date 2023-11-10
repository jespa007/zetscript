#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::StringScriptObject;

StringScriptObject *returnNewStringObject(ZetScript *_zs){

	// instance new StringScriptObject using ZetScript context
	StringScriptObject *string_object=new StringScriptObject(_zs,"Hello World!");

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