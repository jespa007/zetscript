#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::zs_float;

// c function
void funParamFloat(ScriptEngine *_se, zs_float *_number){
    printf("Result : %f\n",*_number);
}

int main(){
	ScriptEngine se;

    se.registerFunction("funParamFloat",funParamFloat);

    // call c function 
    se.compileAndRun(
        "funParamFloat(10.5);"
 	);


 	return 0;
}

