#include "zetscript.h"

zetscript::zs_int returnInteger(
    zetscript::ScriptEngine *_script_engine
){
    return 10;
}

int main(){
	zetscript::ScriptEngine script_engine;

	// Registers the C function
    script_engine.registerFunction("returnInteger",returnInteger);

    // // Compiles and runs a script
    script_engine.compileAndRun(
        "Console::outln(\"result : \"+returnInteger());"
 	);
    return 0;
}