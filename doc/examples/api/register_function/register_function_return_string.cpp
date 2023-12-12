#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::String;

String returnString(ScriptEngine *_script_engine){
    return "Hello world (String)";
}

int main(){
	ScriptEngine script_engine;

	// Registers function interface
    script_engine.registerFunction("returnString",returnString);

    // Evaluates a script that calls the registered function and prints its result by console
    script_engine.eval(
        "Console::outln(\"result : \"+returnString());"
 	);
    return 0;
}

