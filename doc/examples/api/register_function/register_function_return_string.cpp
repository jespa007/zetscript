#include "zetscript.h"

zetscript::String returnString(
    zetscript::ScriptEngine *_script_engine
){
    return "Hello world (String)";
}

int main(){
	zetscript::ScriptEngine script_engine;

	// Registers the C function
    script_engine.registerFunction("returnString",returnString);

    // // Compiles and runs a script
    script_engine.compileAndRun(
        "Console::outln(\"result : \"+returnString());"
 	);
    return 0;
}

