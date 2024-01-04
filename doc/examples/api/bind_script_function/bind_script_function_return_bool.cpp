#include "zetscript.h"

int main()
{
	zetscript::ScriptEngine script_engine;

    // Evaluates ZetScript function 'returnBoolean' that returns 'Boolean' value
    script_engine.compile(
 		"function returnBoolean(){\n"
        "    return true;\n"
        "}\n"
 	);

    // It binds 'returnBoolean' as 'bool(void)'
    auto returnBoolean=script_engine.bindScriptFunction<bool()>("returnBoolean");

    // Calls ZetScript function and prints return value by console.
    printf("result : %s\n",returnBoolean()?"true":"false");

 	return 0;
}

