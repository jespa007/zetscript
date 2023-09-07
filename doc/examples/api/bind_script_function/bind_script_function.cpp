#include "zetscript.h"

int main(){

	zetscript::ZetScript zs;

    // Evaluates ZetScript function 'sayHelloWorld' that prints a "Hello world" by console
    zs.eval(
 		"function sayHelloWorld(){\n"
        "    Console::outln(\"Hello world\");\n"
        "}\n"
 	);

    // It binds 'sayHelloWorld' as 'void(void)'
    auto sayHelloWorld=zs.bindScriptFunction<void(void)>("sayHelloWorld");

    // Calls ZetScript function
    sayHelloWorld();

 	return 0;
}

