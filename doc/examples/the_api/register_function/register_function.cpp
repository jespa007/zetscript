#include "zetscript.h"

using zetscript::ZetScript;

// ZetScript C++ interface function
void sayHelloWorld(ZetScript *_zs){
    printf("Hello world\n");
}

int main(){
	ZetScript zs;

	// Registers sayHelloWorld as 'sayHelloWorld' symbol name
    zs.registerFunction("sayHelloWorld",sayHelloWorld);

    // Evaluates a script where it calls 'sayHelloWorld' function
    zs.eval(
        "sayHelloWorld();"
 	);

    return 0;
}

