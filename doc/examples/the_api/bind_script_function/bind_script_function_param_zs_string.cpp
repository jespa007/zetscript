#include "zetscript.h"

using zetscript::zs_string;

int main()
{
	zetscript::ZetScript zs;

    // Evaluates ZetScript function 'concat' that accepts two 'String' values as parameter
    zs.eval(
 		"function concat(_value1, _value2){\n"
        "    Console::outln(\"result : \"+_value1+\" \"+_value2);\n"
        "}\n"
 	);

    // It binds 'concat' as '(void *)(zs_string *, zs_string *)'
    auto concat=zs.bindScriptFunction<void(zs_string * _value1, zs_string * _value2)>("concat");

    // Prepare param values
    zs_string value1="Hello";
    zs_string value2="World";

    // Calls ZetScript function by value
    concat(&value1,&value2);


 	return 0;
}

