#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::ScriptFunction;

// c function
void paramFunction(ScriptEngine *_script_engine, ScriptFunction *_script_function){

	// bind script function to be callable from C
    auto function=_script_engine->bindScriptFunction<void(void)>(_script_function);

    // call script function
    function();
}

int main(){
	ScriptEngine script_engine;

    script_engine.registerFunction("paramFunction",paramFunction);

    // call c function with string param
    script_engine.eval(
        "paramFunction(function(){\n"
    	"	Console::outln(\"Hello world\");\n"
    	"});"
 	);


 	return 0;
}

