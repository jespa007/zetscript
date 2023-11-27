#include "zetscript.h"

using zetscript::zs_float;

int main()
{
	zetscript::ZetScript zs;

    // Evaluates ZetScript function 'returnFloat' that returns 'Float' value
    zs.eval(
 		"function returnFloat(){\n"
        "    return 10.5;\n"
        "}\n"
 	);

    // It binds 'returnFloat' as 'zs_float(void)'
    auto returnFloat=zs.bindFunction<zs_float()>("returnFloat");

    // Calls ZetScript function and prints return value by console.
    printf("result : %f\n",returnFloat());

 	return 0;
}

