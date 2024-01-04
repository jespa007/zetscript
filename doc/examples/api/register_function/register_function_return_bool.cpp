#include "zetscript.h"

bool returnBoolean(
    zetscript::ScriptEngine *_script_engine
){
    return true;
}

int main(){

	zetscript::ScriptEngine script_engine;

	// Registers function
    script_engine.registerFunction("returnBoolean",returnBoolean);

    // Evaluates a script that calls the registered function and prints its result by console
    script_engine.compileAndRun(
        "Console::outln(\"result : \"+returnBoolean());"
 	);
    return 0;
}