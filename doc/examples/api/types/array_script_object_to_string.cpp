#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::ArrayScriptObject;
using zetscript::zs_int;
using zetscript::zs_float;
using zetscript::String;

// print array contents
void printArray(ScriptEngine *_script_engine,ArrayScriptObject *_array_object){
	printf("Array contents : %s",_array_object->toString().toConstChar());
}

int main(){
	zetscript::ScriptEngine script_engine;

	// registers printArray
	script_engine.registerFunction("printArray",printArray);

	// calls printArray
	script_engine.eval(
        "printArray(["
            "0"                 
            ",10.5"             
            ",\"Hello World!\"" 
        "])");
	
	return 0;
}