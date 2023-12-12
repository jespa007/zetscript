#include "zetscript.h"

int main(){

	zetscript::ScriptEngine zs;

    // Evaluates ScriptEngine function 'sayHelloWorld' that prints a "Hello world" by console
    zs.compile(
 		"function sayHelloWorld(){\n"
        "    Console::outln(\"Hello world\");\n"
        "}\n"
 	);

    // It binds 'sayHelloWorld' as 'void(void)'
    auto sayHelloWorld=zs.bindScriptFunction<void(void)>("sayHelloWorld");

    // Calls ScriptEngine function
    sayHelloWorld();

 	return 0;
}

