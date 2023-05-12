#include "zetscript.h"

using zetscript::ZetScript;

// ZetScript register function 'sayHelloWorld'
void sayHelloWorld(ZetScript *_zs){
    printf("Hello world\n");
}

int main(){
	ZetScript zs;

	// register function 'sayHelloWorld' named as 'sayHelloWorld'
    zs.registerFunction("sayHelloWorld",sayHelloWorld);

    // Eval script that calls native function 'sayHelloWorld'
    zs.eval(
        "sayHelloWorld();"
 	);

    return 0;
}

