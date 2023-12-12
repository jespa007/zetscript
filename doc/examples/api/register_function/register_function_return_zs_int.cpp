#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::zs_int;

zs_int returnInteger(ScriptEngine *_se){
    return 10;
}

int main(){
	ScriptEngine se;

	// Registers function interface
    se.registerFunction("returnInteger",returnInteger);

    // Evaluates a script that calls the registered function and prints its result by console
    se.compileAndRun(
        "Console::outln(\"result : \"+returnInteger());"
 	);
    return 0;
}