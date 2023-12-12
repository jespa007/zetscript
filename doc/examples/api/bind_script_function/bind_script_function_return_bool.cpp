#include "zetscript.h"

int main()
{
	zetscript::ScriptEngine se;

    // Evaluates ScriptEngine function 'returnBoolean' that returns 'Boolean' value
    se.compile(
 		"function returnBoolean(){\n"
        "    return true;\n"
        "}\n"
 	);

    // It binds 'returnBoolean' as 'bool(void)'
    auto returnBoolean=se.bindScriptFunction<bool()>("returnBoolean");

    // Calls ScriptEngine function and prints return value by console.
    printf("result : %s\n",returnBoolean()?"true":"false");

 	return 0;
}

