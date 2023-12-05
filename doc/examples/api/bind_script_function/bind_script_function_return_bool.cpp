#include "zetscript.h"

int main()
{
	zetscript::ZetScript zs;

    // Evaluates ZetScript function 'returnBoolean' that returns 'Boolean' value
    zs.compile(
 		"function returnBoolean(){\n"
        "    return true;\n"
        "}\n"
 	);

    // It binds 'returnBoolean' as 'bool(void)'
    auto returnBoolean=zs.bindScriptFunction<bool()>("returnBoolean");

    // Calls ZetScript function and prints return value by console.
    printf("result : %s\n",returnBoolean()?"true":"false");

 	return 0;
}

