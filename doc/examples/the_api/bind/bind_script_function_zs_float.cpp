#include "zetscript.h"
#include <iostream>

using zetscript::zs_float;

int main(int argc, char *argv[])
{
	zetscript::ZetScript zs;

    zs.eval(
 		"function add(_a,_b){\n"
        "    return _a+_b;\n"
        "}\n"
 	);

    auto add=zs.bindScriptFunction<zs_float(zs_float *,zs_float *)>("add");
    zs_float f1=10.2;
    zs_float f2=10.3;

    std::cout << "result : " << std::to_string(add(&f1,&f2)) << std::endl;

 	return 0;
}

