#include "zetscript.h"

int main(){

	zetscript::ZetScript zs;

    // eval script that defines "sayHelloWorld" script function
    zs.eval(
 		"function sayHelloWorld(){\n"
        "    Console::outln(\"Hello world\");\n"
        "}\n"
 	);

    // binds script function "sayHelloWorld" with no parameter and no return
    auto sayHelloWorld=zs.bindScriptFunction<void(void)>("sayHelloWorld");

    // call script function
    sayHelloWorld();

 	return 0;
}

