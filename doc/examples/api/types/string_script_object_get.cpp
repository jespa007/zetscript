#include "zetscript.h"

void printString(
	zetscript::ScriptEngine *_script_engine
	,zetscript::StringScriptObject *_string_object
){
	printf("%s\n",_string_object->get().toConstChar());
}

int main(){
	zetscript::ScriptEngine script_engine;

	// registers printString
	script_engine.registerFunction("printString",printString);

	// calls printString
	script_engine.compileAndRun("printString(\"Hello World!\")");
	
	return 0;
}