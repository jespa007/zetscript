#include "zetscript.h"
#include <iostream>

using zetscript::zs_float;

int main(int argc, char *argv[])
{
	zetscript::ZetScript zs;

    zs.eval(
 		"class A{\n"
        "    print(){\n"
        "       Console::outln(\"Call from instanced object\")"
        "   }\n"
        "}\n"
        "var a=new A();\n"
 	);

    auto a_print=zs.bindScriptFunction<void(void)>("a.print");

    a_print();

 	return 0;
}

