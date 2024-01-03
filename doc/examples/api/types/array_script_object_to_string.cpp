#include "zetscript.h"

// print array contents
void printArray(
	zetscript::ScriptEngine *_script_engine
	,zetscript::ArrayScriptObject *_array_object
){
	printf("Array contents : %s",_array_object->toString().toConstChar());
}

int main(){
	zetscript::ScriptEngine script_engine;

	// registers printArray
	script_engine.registerFunction("printArray",printArray);

	// calls printArray
	script_engine.compileAndRun(
        "printArray(["
            "0"                 
            ",10.5"             
            ",\"Hello World!\"" 
        "])");
	
	return 0;
}