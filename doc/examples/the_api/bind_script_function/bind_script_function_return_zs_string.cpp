#include "zetscript.h"

using zetscript::zs_string;

int main()
{
	zetscript::ZetScript zs;

    // Evaluates ZetScript function 'returnString' that returns 'Boolean' value
    zs.eval(
 		"function returnString(){\n"
        "    return \"Hello World\";\n"
        "}\n"
 	);

    // It binds 'returnString' as 'zs_string(void)'
    auto returnString=zs.bindScriptFunction<zs_string()>("returnString");

    // Calls ZetScript function and prints return value by console.
    printf("result : %s\n",returnString().c_str());

 	return 0;
}

