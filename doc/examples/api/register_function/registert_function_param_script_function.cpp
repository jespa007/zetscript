#include "zetscript.h"

// c function
void paramFunction(
    zetscript::ScriptEngine *_script_engine
    , zetscript::ScriptFunction *_script_function
){
	// bind script function to be callable from C
    auto function=_script_engine->bindScriptFunction<void(void)>(_script_function);

    // call script function
    function();
}

int main(){
	zetscript::ScriptEngine script_engine;

    script_engine.registerFunction("paramFunction",paramFunction);

    // call c function with string param
    script_engine.compileAndRun(
        "paramFunction(function(){\n"
    	"	Console::outln(\"Hello world\");\n"
    	"});"
 	);


 	return 0;
}

