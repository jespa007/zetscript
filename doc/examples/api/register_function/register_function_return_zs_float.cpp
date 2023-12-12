#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::zs_float;

zs_float returnFloat(ScriptEngine *_se){
    return 10.5;
}

int main(){
	ScriptEngine se;

	// Registers function interface
    se.registerFunction("returnFloat",returnFloat);

    // Evaluates a script that calls the registered function and prints its result by console
    se.compileAndRun(
        "Console::outln(\"result : \"+returnFloat());"
 	);
    return 0;
}

