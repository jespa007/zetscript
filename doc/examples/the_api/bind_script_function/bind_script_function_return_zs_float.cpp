#include "zetscript.h"

using zetscript::zs_float;

int main()
{
	zetscript::ZetScript zs;

    // eval script that defines "returnFloat" script function
    zs.eval(
 		"function returnFloat(){\n"
        "    return 10.5;\n"
        "}\n"
 	);

    // binds script function "returnFloat" that returns float value 
    auto returnFloat=zs.bindScriptFunction<zs_float()>("returnFloat");

    // call script function 
    printf("result : %f\n",returnFloat());

 	return 0;
}

