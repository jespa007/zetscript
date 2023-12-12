#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::StringScriptObject;

void printString(ScriptEngine *_script_engine,StringScriptObject *_string_object){
	printf("%s\n",_string_object->get().toConstChar());
}

int main(){
	zetscript::ScriptEngine script_engine;

	// registers printString
	script_engine.registerFunction("printString",printString);

	// calls printString
	script_engine.eval("printString(\"Hello World!\")");
	
	return 0;
}