#include "zetscript.h"

using zetscript::zs_int;

int main(){
	zetscript::ZetScript zs;

    // Evaluates ZetScript function 'add' that prints the result of the operation '+' from two arguments
    zs.eval(
 		"function printAdd(_value1,_value2){\n"
        "    Console::outln(\"result : \"+(_value1 + _value2));\n"
        "}\n"
 	);

    // It binds 'printAdd' as '(void *)(zs_int, zs_int)' (by value)
    auto printAddByValue=zs.bindScriptFunction<void(zs_int _value1, zs_int _value2)>("printAdd");

    // Calls binded ZetScript function with parameters by value
    printAddByValue(10,10);

 	return 0;
}