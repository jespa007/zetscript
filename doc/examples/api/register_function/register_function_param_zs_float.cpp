#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::zs_float;

// c function
void funParamFloat(ScriptEngine *_zs, zs_float *_number){
    printf("Result : %f\n",*_number);
}

int main(){
	ScriptEngine zs;

    zs.registerFunction("funParamFloat",funParamFloat);

    // call c function 
    zs.compileAndRun(
        "funParamFloat(10.5);"
 	);


 	return 0;
}

