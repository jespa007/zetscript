#include "zetscript.h"

zetscript::zs_int returnInteger(
    zetscript::ScriptEngine *_script_engine
){
    return 10;
}

int main(){
	zetscript::ScriptEngine script_engine;

	// Registers function interface
    script_engine.registerFunction("returnInteger",returnInteger);

    // Evaluates a script that calls the registered function and prints its result by console
    script_engine.compileAndRun(
        "Console::outln(\"result : \"+returnInteger());"
 	);
    return 0;
}