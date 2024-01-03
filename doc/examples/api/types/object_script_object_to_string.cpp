#include "zetscript.h"

// print array contents
void printObject(
	zetscript::ScriptEngine *_script_engine
	,zetscript::ObjectScriptObject *_object
){
	printf("ScriptObject contents : %s",_object->toString().toConstChar());
}

int main(){
	zetscript::ScriptEngine script_engine;

	// registers printObject
	script_engine.registerFunction("printObject",printObject);

	// calls printObject
	script_engine.compileAndRun(
        "printObject({"
            "\"key1\":0"                 
            ",\"key2\":10.5"             
            ",\"key3\":\"Hello World!\"" 
        "})");
	
	return 0;
}