#include "zetscript.h"

int main(){

	zetscript::ScriptEngine se;

    // Evaluates ScriptEngine function 'sayHelloWorld' that prints a "Hello world" by console
    se.compile(
 		"function sayHelloWorld(){\n"
        "    Console::outln(\"Hello world\");\n"
        "}\n"
 	);

    // It binds 'sayHelloWorld' as 'void(void)'
    auto sayHelloWorld=se.bindScriptFunction<void(void)>("sayHelloWorld");

    // Calls ScriptEngine function
    sayHelloWorld();

 	return 0;
}

