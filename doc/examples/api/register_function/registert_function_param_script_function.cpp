#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::Function;

// c function
void paramFunction(ZetScript *_zs, Function *_script_function){

	// bind script function to be callable from C
    auto function=_zs->bindFunction<void(void)>(_script_function);

    // call script function
    function();
}

int main(){
	ZetScript zs;

    zs.registerFunction("paramFunction",paramFunction);

    // call c function with string param
    zs.eval(
        "paramFunction(function(){\n"
    	"	Console::outln(\"Hello world\");\n"
    	"});"
 	);


 	return 0;
}

