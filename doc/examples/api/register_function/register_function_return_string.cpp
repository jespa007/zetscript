#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::String;

String returnString(ScriptEngine *_se){
    return "Hello world (String)";
}

int main(){
	ScriptEngine se;

	// Registers function interface
    se.registerFunction("returnString",returnString);

    // Evaluates a script that calls the registered function and prints its result by console
    se.compileAndRun(
        "Console::outln(\"result : \"+returnString());"
 	);
    return 0;
}

