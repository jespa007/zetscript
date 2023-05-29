#include "zetscript.h"

using zetscript::zs_int;

int main()
{
	zetscript::ZetScript zs;

    // Evaluates ZetScript function 'returnInteger' that returns 'Integer' value
    zs.eval(
 		"function returnInteger(){\n"
        "    return 10;\n"
        "}\n"
 	);

    // It binds 'returnInteger' as 'zs_int(void)'
    auto returnInteger=zs.bindScriptFunction<zs_int()>("returnInteger");

    // Calls ZetScript function and prints return value by console.
    printf("result : %ld\n",returnInteger());

 	return 0;
}

