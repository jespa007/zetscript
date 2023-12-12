#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::ArrayScriptObject;
using zetscript::zs_int;
using zetscript::zs_float;
using zetscript::String;

// print array contents
void printArray(ScriptEngine *_se,ArrayScriptObject *_array_object){
	printf("Array contents : %s",_array_object->toString().toConstChar());
}

int main(){
	zetscript::ScriptEngine se;

	// registers printArray
	se.registerFunction("printArray",printArray);

	// calls printArray
	se.compileAndRun(
        "printArray(["
            "0"                 
            ",10.5"             
            ",\"Hello World!\"" 
        "])");
	
	return 0;
}