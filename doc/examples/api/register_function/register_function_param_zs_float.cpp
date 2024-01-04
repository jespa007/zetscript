#include "zetscript.h"

// c function
void funParamFloat(
    zetscript::ScriptEngine *_script_engine
    ,zetscript::zs_float *_number
){
    printf("Result : %f\n",*_number);
}

int main(){
	zetscript::ScriptEngine script_engine;

    script_engine.registerFunction("funParamFloat",funParamFloat);

    // call c function 
    script_engine.compileAndRun(
        "funParamFloat(10.5);"
 	);


 	return 0;
}

