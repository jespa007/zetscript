#include "zetscript.h"

using zetscript::StringObject;

int main()
{
	zetscript::ZetScript zs;

    // Evaluates ZetScript function 'printConcat' that prints the result of the operation '+' from two arguments
    zs.eval(
 		"function printConcat(_value1, _value2){\n"
        "    Console::outln(\"result : \"+_value1+\" \"+_value2);\n"
        "}\n"
 	);

    // It binds 'printConcat' as '(void *)(StringObject *, StringObject *)'
    auto printConcat=zs.bindFunction<void(StringObject * _value1, StringObject * _value2)>("printConcat");

    // Prepare param values
    auto value1=zs.newStringObject("Hello");
    auto value2=zs.newStringObject("World");

    // Calls binded ZetScript function with parameters
    printConcat(value1,value2);


 	return 0;
}

