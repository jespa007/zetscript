#include "zetscript.h"

using zetscript::zs_float;

int main()
{
	zetscript::ZetScript zs;

    // Evaluates ZetScript function 'add' that accepts two 'Float' values as parameter
    zs.eval(
 		"function add(_value1,_value2){\n"
        "    Console::outln(\"result : \"+_value1 + _value2);\n"
        "}\n"
 	);

    // It binds 'add' as '(void *)(zs_int, zs_int *)'
    auto add=zs.bindScriptFunction<void(zs_float *_value1, zs_float *_value2)>("add");

    // Prepare parameter values 
    zs_float value1=3.5;
    zs_float value2=10.7;

    // Calls ZetScript function
    add(&value1,&value2);

 	return 0;
}

