#include "zetscript.h"

int main()
{
	zetscript::ZetScript zs;

    // Evaluates ZetScript function 'returnString' that returns 'String' value
    zs.eval(
 		"function returnString(){\n"
        "    return \"Hello World\";\n"
        "}\n"
 	);

    // It binds 'returnString' as 'const char *(void)'
    auto returnString=zs.bindScriptFunction<const char *()>("returnString");

    // Calls ZetScript function and prints return value by console.
    printf("result : %s\n",returnString());

 	return 0;
}

