#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::StringScriptObject;

void printString(ScriptEngine *_se,StringScriptObject *_string_object){
	printf("%s\n",_string_object->get().toConstChar());
}

int main(){
	zetscript::ScriptEngine se;

	// registers printString
	se.registerFunction("printString",printString);

	// calls printString
	se.compileAndRun("printString(\"Hello World!\")");
	
	return 0;
}