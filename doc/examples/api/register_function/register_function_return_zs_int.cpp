#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::zs_int;

zs_int returnInteger(ScriptEngine *_zs){
    return 10;
}

int main(){
	ScriptEngine zs;

	// Registers function interface
    zs.registerFunction("returnInteger",returnInteger);

    // Evaluates a script that calls the registered function and prints its result by console
    zs.compileAndRun(
        "Console::outln(\"result : \"+returnInteger());"
 	);
    return 0;
}