#include "zetscript.h"

using zetscript::ScriptEngine;

bool returnBoolean(ScriptEngine *_se){
    return true;
}

int main(){

	ScriptEngine se;

	// Registers function
    se.registerFunction("returnBoolean",returnBoolean);

    // Evaluates a script that calls the registered function and prints its result by console
    se.compileAndRun(
        "Console::outln(\"result : \"+returnBoolean());"
 	);
    return 0;
}