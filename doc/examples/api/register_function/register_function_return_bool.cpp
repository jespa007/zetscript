#include "zetscript.h"

using zetscript::ScriptEngine;

bool returnBoolean(ScriptEngine *_zs){
    return true;
}

int main(){

	ScriptEngine zs;

	// Registers function
    zs.registerFunction("returnBoolean",returnBoolean);

    // Evaluates a script that calls the registered function and prints its result by console
    zs.compileAndRun(
        "Console::outln(\"result : \"+returnBoolean());"
 	);
    return 0;
}