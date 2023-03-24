#include "zetscript.h"
#include <iostream>

using zetscript::zs_string;

int main(int argc, char *argv[])
{
	zetscript::ZetScript zs;

    zs.eval(
 		"function add(_a,_b){\n"
        "    return _a+_b;\n"
        "}\n"
 	);

    auto add=zs.bindScriptFunction<zs_string(zs_string *,zs_string *)>("add");
    zs_string s1="Hello";
    zs_string s2="World";

    std::cout << "result : " << add(&s1,&s2).c_str() << std::endl;

 	return 0;
}

