#include "zetscript.h"

using zetscript::ScriptEngine;

// The C function to register that prints parameter value by console
void paramBool(ScriptEngine *_script_engine, bool *_bool){
    printf("Result : %s\n",*_bool?"true":"false");
}

int main(){
	ScriptEngine script_engine;

    // Register C function
    script_engine.registerFunction("paramBool",paramBool);

    // Evaluates the script that calls C function with 'true' value as argument
    script_engine.compileAndRun(
        "paramBool(true);"
 	);


 	return 0;
}

