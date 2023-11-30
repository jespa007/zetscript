#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::ScriptFunction;


// The C function to register that binds and calls the script function passed by parameter
void paramFunction(ZetScript *_zs, ScriptFunction *_script_function){

    // bind script function to make it callable
    auto script_function=_zs->bindFunction<void ()>(_script_function);

    // call script function
    script_function();
}

int main(){
	ZetScript zs;

    // register C function
    zs.registerFunction("paramFunction",paramFunction);

    // Evaluates the script that calls C function with anonymous function as argument
    zs.eval(
        "paramFunction(function(){\n"
        "    Console::out(\"Calling from script function\")\n"
        "})\n"
 	);


 	return 0;
}

