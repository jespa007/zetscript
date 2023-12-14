#include "zetscript.h"

using zetscript::zs_float;

int main()
{
	zetscript::ScriptEngine script_engine;

    // Evaluates ZetScript function 'returnFloat' that returns 'Float' value
    script_engine.compile(
 		"function returnFloat(){\n"
        "    return 10.5;\n"
        "}\n"
 	);

    // It binds 'returnFloat' as 'zs_float(void)'
    auto returnFloat=script_engine.bindScriptFunction<zs_float()>("returnFloat");

    // Calls ZetScript function and prints return value by console.
    printf("result : %f\n",returnFloat());

 	return 0;
}

