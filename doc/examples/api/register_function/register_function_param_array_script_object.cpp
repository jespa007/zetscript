#include "zetscript.h"

// c function expects an array of integers
void paramArrayScriptObject(
    zetscript::ScriptEngine *_script_engine
    ,zetscript::ArrayScriptObject *_array
){
	printf("Values in array => ");
    for(int i=0; i < _array->length(); i++){
        printf(" %i",(int)_array->get<zetscript::zs_int>(i));
    }
    printf("\n");
}

int main(){
	zetscript::ScriptEngine script_engine;

    script_engine.registerFunction("paramArrayScriptObject",paramArrayScriptObject);

    // call c function with string param
    script_engine.compileAndRun(
        "paramArrayScriptObject(["
        "    0,1,2,3,4,5"
        "]);"
 	);

 	return 0;
}