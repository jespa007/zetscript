#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::ScriptFunction;

// c function
void paramFunction(ZetScript *_zs, ScriptFunction *_script_function){

	// bind script function to be callable from C
    auto function=_zs->bindFunction<void(void)>(_script_function);

    // call script function
    function();
}

int main(){
	ZetScript zs;

    zs.registerFunction("paramFunction",paramFunction);

    // call c function with string param
    zs.compileAndRun(
        "paramFunction(function(){\n"
    	"	Console::outln(\"Hello world\");\n"
    	"});"
 	);


 	return 0;
}

