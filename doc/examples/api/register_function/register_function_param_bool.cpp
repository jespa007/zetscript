#include "zetscript.h"


void paramBool(
    zetscript::ScriptEngine *_script_engine
    , bool *_bool
){
    printf("Result : %s\n",*_bool?"true":"false");
}

int main(){
	zetscript::ScriptEngine script_engine;

    // Register C function
    script_engine.registerFunction("paramBool",paramBool);

    // Compiles and runs script
    script_engine.compileAndRun(
        "paramBool(true);"
 	);


 	return 0;
}

