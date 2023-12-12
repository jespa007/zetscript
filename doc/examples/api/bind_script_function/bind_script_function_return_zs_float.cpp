#include "zetscript.h"

using zetscript::zs_float;

int main()
{
	zetscript::ScriptEngine zs;

    // Evaluates ScriptEngine function 'returnFloat' that returns 'Float' value
    zs.compile(
 		"function returnFloat(){\n"
        "    return 10.5;\n"
        "}\n"
 	);

    // It binds 'returnFloat' as 'zs_float(void)'
    auto returnFloat=zs.bindScriptFunction<zs_float()>("returnFloat");

    // Calls ScriptEngine function and prints return value by console.
    printf("result : %f\n",returnFloat());

 	return 0;
}

