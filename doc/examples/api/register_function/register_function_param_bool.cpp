#include "zetscript.h"

using zetscript::ScriptEngine;

// The C function to register that prints parameter value by console
void paramBool(ScriptEngine *_zs, bool *_bool){
    printf("Result : %s\n",*_bool?"true":"false");
}

int main(){
	ScriptEngine zs;

    // Register C function
    zs.registerFunction("paramBool",paramBool);

    // Evaluates the script that calls C function with 'true' value as argument
    zs.compileAndRun(
        "paramBool(true);"
 	);


 	return 0;
}

