#include "zetscript.h"

using zetscript::ObjectScriptObject;

int main()
{
	zetscript::ZetScript zs;

    // Evaluates ZetScript function 'concat' that prints the result of the operation '+' from two arguments
    zs.eval(
 		"function concat(_value1, _value2){\n"
        "    Console::outln(\"result : \"+_value1+_value2);\n"
        "}\n"
 	);

    // It binds 'concat' as '(void *)(ArrayScriptObject *, ArrayScriptObject *)'
    auto concat=zs.bindScriptFunction<void(ObjectScriptObject * _value1, ObjectScriptObject * _value2)>("concat");

    // Prepare param values
    auto value1=zs.newObjectScriptObject();
    auto value2=zs.newObjectScriptObject();



    // Calls ZetScript function by value
    concat(value1,value2);


 	return 0;
}

