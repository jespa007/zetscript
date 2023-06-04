#include "zetscript.h"

using zetscript::ArrayScriptObject;
using zetscript::zs_int;
using zetscript::zs_float;
using zetscript::zs_string;

int main()
{
	zetscript::ZetScript zs;

    // Evaluates ZetScript function 'concat' that prints the result of the operation '+' from two arguments
    zs.eval(
 		"function concat(_value1, _value2){\n"
        "    Console::outln(\"result : \"+(_value1+_value2));\n"
        "}\n"
 	);

    // It binds 'concat' as '(void *)(ArrayScriptObject *, ArrayScriptObject *)'
    auto concat=zs.bindScriptFunction<void(ArrayScriptObject * _value1, ArrayScriptObject * _value2)>("concat");

    // Prepare param values
    auto array1=zs.newArrayScriptObject();
    auto array2=zs.newArrayScriptObject();

// push values for array1
   array1->push<bool>(false);
   array1->push<zs_int>(10);
   array1->push<const char *>("Hello");

   // push values for array2
   array2->push<bool>(true);
   array2->push<zs_float>(20.5);
   array2->push<const char *>("World");


    // Calls ZetScript function by value
    concat(array1,array2);


 	return 0;
}

