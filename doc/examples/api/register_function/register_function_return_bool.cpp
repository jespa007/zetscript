#include "zetscript.h"

using zetscript::ScriptEngine;

bool returnBoolean(ScriptEngine *_script_engine){
    return true;
}

int main(){

	ScriptEngine script_engine;

	// Registers function
    script_engine.registerFunction("returnBoolean",returnBoolean);

    // Evaluates a script that calls the registered function and prints its result by console
    script_engine.compileAndRun(
        "Console::outln(\"result : \"+returnBoolean());"
 	);
    return 0;
}