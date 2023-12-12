#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::zs_float;

zs_float returnFloat(ScriptEngine *_zs){
    return 10.5;
}

int main(){
	ScriptEngine zs;

	// Registers function interface
    zs.registerFunction("returnFloat",returnFloat);

    // Evaluates a script that calls the registered function and prints its result by console
    zs.compileAndRun(
        "Console::outln(\"result : \"+returnFloat());"
 	);
    return 0;
}

