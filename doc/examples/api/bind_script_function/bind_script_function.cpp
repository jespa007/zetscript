#include "zetscript.h"

int main(){

	zetscript::ScriptEngine script_engine;

    // Evaluates ZetScript function 'sayHelloWorld' that prints a "Hello world" by console
    script_engine.compile(
 		"function sayHelloWorld(){\n"
        "    Console::outln(\"Hello world\");\n"
        "}\n"
 	);

    // It binds 'sayHelloWorld' as 'void(void)'
    auto sayHelloWorld=script_engine.bindScriptFunction<void(void)>("sayHelloWorld");

    // Calls ZetScript function
    sayHelloWorld();

 	return 0;
}

