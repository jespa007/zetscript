#include "zetscript.h"

using zetscript::zs_float;

int main()
{
	zetscript::ScriptEngine se;

    // Evaluates ScriptEngine function 'returnFloat' that returns 'Float' value
    se.compile(
 		"function returnFloat(){\n"
        "    return 10.5;\n"
        "}\n"
 	);

    // It binds 'returnFloat' as 'zs_float(void)'
    auto returnFloat=se.bindScriptFunction<zs_float()>("returnFloat");

    // Calls ScriptEngine function and prints return value by console.
    printf("result : %f\n",returnFloat());

 	return 0;
}

