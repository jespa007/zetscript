#include "zetscript.h"

int main(){

	zetscript::ScriptEngine script_engine;

    // Evaluates ScriptEngine function 'sayHelloWorld' that prints a "Hello world" by console
    script_engine.compile(
 		"function sayHelloWorld(){\n"
        "    Console::outln(\"Hello world\");\n"
        "}\n"
 	);

    // It binds 'sayHelloWorld' as 'void(void)'
    auto sayHelloWorld=script_engine.bindScriptFunction<void(void)>("sayHelloWorld");

    // Calls ScriptEngine function
    sayHelloWorld();

 	return 0;
}

