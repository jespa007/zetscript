#include "zetscript.h"

int main()
{
	zetscript::ZetScript zs;

    // eval script that defines "sayHelloWorld" script function
    zs.eval(
 		"function returnBoolean(){\n"
        "    return true;\n"
        "}\n"
 	);

    // binds script function "returnInteger" that returns boolean value
    auto returnBoolean=zs.bindScriptFunction<bool()>("returnBoolean");

    // call script function
    printf("result : %s\n",returnBoolean()?"true":"false");

 	return 0;
}

