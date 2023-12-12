#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::ArrayScriptObject;
using zetscript::zs_int;
using zetscript::zs_float;
using zetscript::String;

// print array contents
void printArray(ScriptEngine *_zs,ArrayScriptObject *_array_object){
	printf("Array contents : %s",_array_object->toString().toConstChar());
}

int main(){
	zetscript::ScriptEngine zs;

	// registers printArray
	zs.registerFunction("printArray",printArray);

	// calls printArray
	zs.compileAndRun(
        "printArray(["
            "0"                 
            ",10.5"             
            ",\"Hello World!\"" 
        "])");
	
	return 0;
}