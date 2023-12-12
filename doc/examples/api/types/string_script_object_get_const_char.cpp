#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::StringScriptObject;

void printString(ScriptEngine *_zs,StringScriptObject *_string_object){
	printf("%s\n",_string_object->getConstChar());
}

int main(){
	zetscript::ScriptEngine zs;

	// registers printString
	zs.registerFunction("printString",printString);

	// calls printString
	zs.compileAndRun("printString(\"Hello World!\")");
	
	return 0;
}