#include "zetscript.h"

using zetscript::zs_string;

int main()
{
	zetscript::ZetScript zs;

    // Evaluates ZetScript function 'concat' that prints the result of the operation '+' from two arguments
    zs.eval(
 		"function printConcat(_value1, _value2){\n"
        "    Console::outln(\"result : \"+_value1+\" \"+_value2);\n"
        "}\n"
 	);

    // It binds 'printConcat' as '(void *)(zs_string *, zs_string *)'
    auto printConcat=zs.bindScriptFunction<void(zs_string * _value1, zs_string * _value2)>("printConcat");

    // Prepare param values
    zs_string value1="Hello";
    zs_string value2="World";

    // Calls ZetScript function by value
    printConcat(&value1,&value2);


 	return 0;
}

