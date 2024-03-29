#include "zetscript.h"

// The C function to register that binds and calls the script function passed by parameter
void paramFunction(
    zetscript::ScriptEngine *_script_engine, 
    zetscript::ScriptFunction *_script_function
){

    // bind script function to make it callable
    auto script_function=_script_engine->bindScriptFunction<void ()>(_script_function);

    // call script function
    script_function();
}

int main(){
	zetscript::ScriptEngine script_engine;

    // register C function
    script_engine.registerFunction("paramFunction",paramFunction);

    // Evaluates the script that calls C function with anonymous function as argument
    script_engine.compileAndRun(
        "paramFunction(function(){\n"
        "    Console::out(\"Calling from script function\")\n"
        "})\n"
 	);


 	return 0;
}

