#include "zetscript.h"

using zetscript::StringScriptObject;

int main()
{
	zetscript::ZetScript zs;

    // Evaluates ZetScript function 'concat' that prints the result of the operation '+' from two arguments
    zs.eval(
 		"function concat(_value1, _value2){\n"
        "    Console::outln(\"result : \"+_value1+\" \"+_value2);\n"
        "}\n"
 	);

    // It binds 'concat' as '(void *)(StringScriptObject *, StringScriptObject *)'
    auto concat=zs.bindScriptFunction<void(StringScriptObject * _value1, StringScriptObject * _value2)>("concat");

    // Prepare param values
    auto value1=zs.newStringScriptObject("Hello");
    auto value2=zs.newStringScriptObject("World");

    // Calls ZetScript function by value
    concat(&value1,&value2);


 	return 0;
}

