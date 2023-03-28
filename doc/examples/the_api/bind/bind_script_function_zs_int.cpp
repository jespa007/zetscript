#include "zetscript.h"
#include <iostream>

using zetscript::zs_int;

int main()
{
	zetscript::ZetScript zs;

    zs.eval(
 		"function add(_a,_b){\n"
        "    return _a+_b;\n"
        "}\n"
 	);

    auto add=zs.bindScriptFunction<zs_int(zs_int,zs_int)>("add");

    std::cout << "result :" << std::to_string(add(10,5)) << std::endl;

 	return 0;
}

