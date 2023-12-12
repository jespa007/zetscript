#include "zetscript.h"

using zetscript::StringScriptObject;

int main()
{
	zetscript::ScriptEngine se;

    // Evaluates ScriptEngine function 'printConcat' that prints the result of the operation '+' from two arguments
    se.compile(
 		"function printConcat(_value1, _value2){\n"
        "    Console::outln(\"result : \"+_value1+\" \"+_value2);\n"
        "}\n"
 	);

    // It binds 'printConcat' as '(void *)(StringScriptObject *, StringScriptObject *)'
    auto printConcat=se.bindScriptFunction<void(StringScriptObject * _value1, StringScriptObject * _value2)>("printConcat");

    // Prepare param values
    auto value1=se.newStringScriptObject("Hello");
    auto value2=se.newStringScriptObject("World");

    // Calls binded ScriptEngine function with parameters
    printConcat(value1,value2);


 	return 0;
}

