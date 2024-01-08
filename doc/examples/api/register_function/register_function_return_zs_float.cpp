#include "zetscript.h"

zetscript::zs_float returnFloat(
    zetscript::ScriptEngine *_script_engine
){
    return 10.5;
}

int main(){
	zetscript::ScriptEngine script_engine;

	// Registers the C function
    script_engine.registerFunction("returnFloat",returnFloat);

    // // Compiles and runs a script
    script_engine.compileAndRun(
        "Console::outln(\"result : \"+returnFloat());"
 	);
    return 0;
}

