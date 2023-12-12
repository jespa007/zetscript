#include "zetscript.h"

int main()
{
	zetscript::ScriptEngine script_engine;

    // Evaluates ScriptEngine function 'returnBoolean' that returns 'Boolean' value
    script_engine.compile(
 		"function returnBoolean(){\n"
        "    return true;\n"
        "}\n"
 	);

    // It binds 'returnBoolean' as 'bool(void)'
    auto returnBoolean=script_engine.bindScriptFunction<bool()>("returnBoolean");

    // Calls ScriptEngine function and prints return value by console.
    printf("result : %s\n",returnBoolean()?"true":"false");

 	return 0;
}

