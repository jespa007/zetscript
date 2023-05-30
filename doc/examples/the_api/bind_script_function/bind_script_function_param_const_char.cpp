#include "zetscript.h"

using zetscript::zs_int;

int main()
{
	zetscript::ZetScript zs;

    // Evaluates ZetScript function 'concat' that accepts two 'String' values as parameter
    zs.eval(
 		"function concat(_value1, _value2){\n"
        "    Console::outln(\"result : \"+_value1+\" \"+_value2);\n"
        "}\n"
 	);

    // It binds 'concat' as '(void *)(const char *, const char *)'
    auto concat=zs.bindScriptFunction<void(const char * _value1, const char * _value2)>("concat");

    // Calls ZetScript function by value
    concat("Hello","World");


 	return 0;
}

