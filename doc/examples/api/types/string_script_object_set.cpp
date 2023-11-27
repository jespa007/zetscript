#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::StringObject;

StringObject *returnString(ZetScript *_zs){
	StringObject *string_object = new StringObject(_zs);
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