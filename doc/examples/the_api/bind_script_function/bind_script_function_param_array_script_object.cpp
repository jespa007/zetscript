#include "zetscript.h"

using zetscript::ArrayScriptObject;

int main()
{
	zetscript::ZetScript zs;

    // Evaluates ZetScript function 'concat' that accepts two 'Array' values as parameter
    zs.eval(
 		"function concat(_value1, _value2){\n"
        "    Console::outln(\"result : \"+_value1+_value2);\n"
        "}\n"
 	);

    // It binds 'concat' as '(void *)(ArrayScriptObject *, ArrayScriptObject *)'
    auto concat=zs.bindScriptFunction<void(ArrayScriptObject * _value1, ArrayScriptObject * _value2)>("concat");

    // Prepare param values
    auto value1=zs.newArrayScriptObject();
    auto value2=zs.newArrayScriptObject();



    // Calls ZetScript function by value
    concat(&value1,&value2);


 	return 0;
}

