#include "zetscript.h"

using zetscript::StringScriptObject;

int main()
{
	zetscript::ZetScript zs;

    // Evaluates ZetScript function 'printConcat' that prints the result of the operation '+' from two arguments
    zs.compile(
 		"function printConcat(_value1, _value2){\n"
        "    Console::outln(\"result : \"+_value1+\" \"+_value2);\n"
        "}\n"
 	);

    // It binds 'printConcat' as '(void *)(StringScriptObject *, StringScriptObject *)'
    auto printConcat=zs.bindScriptFunction<void(StringScriptObject * _value1, StringScriptObject * _value2)>("printConcat");

    // Prepare param values
    auto value1=zs.newStringScriptObject("Hello");
    auto value2=zs.newStringScriptObject("World");

    // Calls binded ZetScript function with parameters
    printConcat(value1,value2);


 	return 0;
}

