#include "zetscript.h"

using zetscript::ZetScript;

// Definition of the native function interface sayHelloWorld
void sayHelloWorld(ZetScript *_zs){
    printf("Hello world\n");
}

int main(){
	ZetScript zs;

	// bind native function sayHelloWorld named as 'sayHelloWorld'
    zs.bindFunction("sayHelloWorld",sayHelloWorld);

    // Eval script that calls native function 'sayHelloWorld'
    zs.eval(
        "sayHelloWorld();"
 	);

    return 0;
}

