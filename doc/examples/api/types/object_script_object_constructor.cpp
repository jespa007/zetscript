#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::DictionaryScriptObject;

DictionaryScriptObject *returnNewObject(ZetScript *_zs){

	// instance new StringScriptObject using ZetScript context
	DictionaryScriptObject *new_object=new DictionaryScriptObject(_zs);

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