#include "zetscript.h"

using zetscript::zs_int;

int main()
{
	zetscript::ZetScript zs;

    // eval script that defines "returnInteger" script function
    zs.eval(
 		"function returnInteger(){\n"
        "    return 10;\n"
        "}\n"
 	);

    // binds script function "returnInteger" that returns integer value
    auto returnInteger=zs.bindScriptFunction<zs_int()>("returnInteger");

    // call script function 
    printf("result : %ld\n",returnInteger());

 	return 0;
}

