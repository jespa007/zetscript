#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::StringScriptObject;

StringScriptObject *returnString(ZetScript *_zs){
	StringScriptObject *string_object = new StringScriptObject(_zs);
	string_object->set("Hello World!");
	return string_object;
}

int main(){
	zetscript::ZetScript zs;

	// registers returnString
	zs.registerFunction("returnString",returnString);

	// prints the value returned by returnString
	zs.eval("Console::outln(returnString())");
	
	return 0;
}