#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::String;

String returnString(ScriptEngine *_zs){
    return "Hello world (String)";
}

int main(){
	ScriptEngine zs;

	// Registers function interface
    zs.registerFunction("returnString",returnString);

    // Evaluates a script that calls the registered function and prints its result by console
    zs.compileAndRun(
        "Console::outln(\"result : \"+returnString());"
 	);
    return 0;
}

