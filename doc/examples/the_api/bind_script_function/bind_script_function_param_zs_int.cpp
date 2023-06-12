#include "zetscript.h"

using zetscript::zs_int;

int main()
{
	zetscript::ZetScript zs;

    // Evaluates ZetScript function 'add' that prints the result of the operation '+' from two arguments
    zs.eval(
 		"function printAdd(_value1,_value2){\n"
        "    Console::outln(\"result : \"+(_value1 + _value2));\n"
        "}\n"
 	);

    // It binds 'printAdd' as '(void *)(zs_int, zs_int)' (by value)
    auto printAddByValue=zs.bindScriptFunction<void(zs_int _value1, zs_int _value2)>("printAdd");

    // It binds 'printAdd' as '(void *)(zs_int *, zs_int *)' (by reference)
    auto printAddByRef=zs.bindScriptFunction<void(zs_int *_value1, zs_int *_value2)>("printAdd");

    // Prepare parameter values 
    zs_int value1=10;
    zs_int value2=10;

    // Calls binded ZetScript function with parameters by value
    printAddByValue(value1,value2);

    // Calls binded ZetScript function with parameters by pointer
    printAddByRef(&value1,&value2);

 	return 0;
}

