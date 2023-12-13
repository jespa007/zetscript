#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::StringScriptObject;

void checkStringLength(ScriptEngine *_script_engine,StringScriptObject *_string_object){
	printf("The length of '%s' is %i\n",_string_object->toString().toConstChar(),_string_object->length());
}

int main(){
	zetscript::ScriptEngine script_engine;

	// registers checkStringLength
	script_engine.registerFunction("checkStringLength",checkStringLength);

	// calls checkStringLength
	script_engine.compileAndRun("checkStringLength(\"Hello World!\")");
	
	return 0;
}