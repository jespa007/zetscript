#include "zetscript.h"

zetscript::String returnString(
    zetscript::ScriptEngine *_script_engine
){
    return "Hello world (String)";
}

int main(){
	zetscript::ScriptEngine script_engine;

	// Registers function interface
    script_engine.registerFunction("returnString",returnString);

    // Evaluates a script that calls the registered function and prints its result by console
    script_engine.compileAndRun(
        "Console::outln(\"result : \"+returnString());"
 	);
    return 0;
}

