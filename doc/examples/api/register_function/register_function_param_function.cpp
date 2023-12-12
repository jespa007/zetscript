#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::ScriptFunction;


// The C function to register that binds and calls the script function passed by parameter
void paramFunction(ScriptEngine *_se, ScriptFunction *_script_function){

    // bind script function to make it callable
    auto script_function=_se->bindScriptFunction<void ()>(_script_function);

    // call script function
    script_function();
}

int main(){
	ScriptEngine se;

    // register C function
    se.registerFunction("paramFunction",paramFunction);

    // Evaluates the script that calls C function with anonymous function as argument
    se.compileAndRun(
        "paramFunction(function(){\n"
        "    Console::out(\"Calling from script function\")\n"
        "})\n"
 	);


 	return 0;
}

