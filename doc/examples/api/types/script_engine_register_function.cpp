#include "zetscript.h"

// ScriptEngine C++ interface function
void sayHelloWorld(zetscript::ScriptEngine *_script_engine){
    printf("Hello world\n");
}

int main(){
	zetscript::ScriptEngine script_engine;

	// Registers sayHelloWorld as 'sayHelloWorld' symbol name
    script_engine.registerFunction("sayHelloWorld",sayHelloWorld);

    // Evaluates a script where it calls 'sayHelloWorld' function
    script_engine.compileAndRun(
        "sayHelloWorld();"
 	);

    return 0;
}

