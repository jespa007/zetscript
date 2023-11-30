#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::StringScriptObject;

StringScriptObject *returnNewStringScriptObject(ZetScript *_zs){

	// instance new StringScriptObject using ZetScript context
	StringScriptObject *string_object=new StringScriptObject(_zs,"Hello World!");

	// ...

	return string_object;

}

int main(){
	zetscript::ZetScript zs;

	// Registers returnNewStringScriptObject
	zs.registerFunction("returnNewStringScriptObject",returnNewStringScriptObject);

	// prints the returning of new string object
	zs.eval("Console::outln(returnNewStringScriptObject())");
	
	return 0;
}