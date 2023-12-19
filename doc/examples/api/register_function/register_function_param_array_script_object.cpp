#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::ArrayScriptObject;
using zetscript::zs_int;
using zetscript::zs_float;

// c function expects an array of integers
void paramArrayScriptObject(ScriptEngine *_script_engine, ArrayScriptObject *_array){
	printf("Values in array => ");
    for(int i=0; i < _array->length(); i++){
        printf(" %i",(int)_array->get<zs_int>(i));
    }
    printf("\n");
}

int main(){
	ScriptEngine script_engine;

    script_engine.registerFunction("paramArrayScriptObject",paramArrayScriptObject);

    // call c function with string param
    script_engine.compileAndRun(
        "paramArrayScriptObject(["
        "    0,1,2,3,4,5"
        "]);"
 	);

 	return 0;
}