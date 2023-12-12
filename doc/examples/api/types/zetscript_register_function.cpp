#include "zetscript.h"

using zetscript::ScriptEngine;

// ScriptEngine C++ interface function
void sayHelloWorld(ScriptEngine *_se){
    printf("Hello world\n");
}

int main(){
	ScriptEngine se;

	// Registers sayHelloWorld as 'sayHelloWorld' symbol name
    se.registerFunction("sayHelloWorld",sayHelloWorld);

    // Evaluates a script where it calls 'sayHelloWorld' function
    se.compileAndRun(
        "sayHelloWorld();"
 	);

    return 0;
}

