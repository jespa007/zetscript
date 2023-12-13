#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::zs_float;

// c function
void funParamFloat(ScriptEngine *_script_engine, zs_float *_number){
    printf("Result : %f\n",*_number);
}

int main(){
	ScriptEngine script_engine;

    script_engine.registerFunction("funParamFloat",funParamFloat);

    // call c function 
    script_engine.compileAndRun(
        "funParamFloat(10.5);"
 	);


 	return 0;
}

