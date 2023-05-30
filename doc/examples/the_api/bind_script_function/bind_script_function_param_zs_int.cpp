#include "zetscript.h"

using zetscript::zs_int;

int main()
{
	zetscript::ZetScript zs;

    // Evaluates ZetScript function 'add' that accepts two 'Integer' values as parameter
    zs.eval(
 		"function add(_value1,_value2){\n"
        "    Console::outln(\"result : \"+_value1 + _value2);\n"
        "}\n"
 	);

    // It binds 'add' as '(void *)(zs_int, zs_int)' (by value)
    auto addByValue=zs.bindScriptFunction<void(zs_int _value1, zs_int _value2)>("add");

    // It binds 'add' as '(void *)(zs_int *, zs_int *)' (by reference)
    auto addByRef=zs.bindScriptFunction<void(zs_int *_value1, zs_int *_value2)>("add");

    // Prepare parameter values 
    zs_int value1=10;
    zs_int value2=10;

    // Calls ZetScript function by value
    addByValue(value1,value2);

    // Calls ZetScript function by reference
    addByRef(&value1,&value2);

 	return 0;
}

