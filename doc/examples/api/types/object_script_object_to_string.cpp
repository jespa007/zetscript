#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::ObjectScriptObject;
using zetscript::zs_int;
using zetscript::zs_float;
using zetscript::String;

// print array contents
void printObject(ScriptEngine *_script_engine,ObjectScriptObject *_object){
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