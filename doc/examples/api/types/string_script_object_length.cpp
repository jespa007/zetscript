#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::StringScriptObject;

void printStringLength(ScriptEngine *_script_engine,StringScriptObject *_string_object){
	printf("The length of '%s' is %i\n",_string_object->toString().toConstChar(),_string_object->length());
}

int main(){
	zetscript::ScriptEngine script_engine;

	// registers printStringLength
	script_engine.registerFunction("printStringLength",printStringLength);

	// calls printStringLength
	script_engine.compileAndRun("printStringLength(\"Hello World!\")");
	
	return 0;
}