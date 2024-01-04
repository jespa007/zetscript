#include "zetscript.h"

zetscript::zs_float returnFloat(
    zetscript::ScriptEngine *_script_engine
){
    return 10.5;
}

int main(){
	zetscript::ScriptEngine script_engine;

	// Registers function interface
    script_engine.registerFunction("returnFloat",returnFloat);

    // Evaluates a script that calls the registered function and prints its result by console
    script_engine.compileAndRun(
        "Console::outln(\"result : \"+returnFloat());"
 	);
    return 0;
}

