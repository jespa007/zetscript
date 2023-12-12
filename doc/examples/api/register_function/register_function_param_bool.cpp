#include "zetscript.h"

using zetscript::ScriptEngine;

// The C function to register that prints parameter value by console
void paramBool(ScriptEngine *_se, bool *_bool){
    printf("Result : %s\n",*_bool?"true":"false");
}

int main(){
	ScriptEngine se;

    // Register C function
    se.registerFunction("paramBool",paramBool);

    // Evaluates the script that calls C function with 'true' value as argument
    se.compileAndRun(
        "paramBool(true);"
 	);


 	return 0;
}

