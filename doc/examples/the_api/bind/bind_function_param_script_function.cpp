#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::ScriptFunction;

// c function
void paramScriptFunction(ZetScript *_zs, ScriptFunction *_script_function){

	// bind script function to be callable from C
    auto function=_zs->bindScriptFunction<void(void)>(_script_function);

    // call script function
    function();
}

int main(){
	ZetScript zs;

    zs.bindFunction("paramScriptFunction",paramScriptFunction);

    // call c function with string param
    zs.eval(
        "paramScriptFunction(function(){\n"
    	"	Console::outln(\"Hello world\");\n"
    	"});"
 	);


 	return 0;
}

