#include "zetscript.h"

using zetscript::ScriptEngine;

// ScriptEngine C++ interface function
void sayHelloWorld(ScriptEngine *_zs){
    printf("Hello world\n");
}

int main(){
	ScriptEngine zs;

	// Registers sayHelloWorld as 'sayHelloWorld' symbol name
    zs.registerFunction("sayHelloWorld",sayHelloWorld);

    // Evaluates a script where it calls 'sayHelloWorld' function
    zs.compileAndRun(
        "sayHelloWorld();"
 	);

    return 0;
}

