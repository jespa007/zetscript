#include "zetscript.h"

int main()
{
	zetscript::ScriptEngine se;

    // Evaluates ScriptEngine function 'paramBoolean' that prints the contents of '_value'
    se.compile(
 		"function paramBoolean(_value){\n"
        "    Console::outln(\"result : \"+_value);\n"
        "}\n"
 	);

    // It binds 'paramBoolean' as '(void *)(bool *)'
    auto paramBoolean=se.bindScriptFunction<void(bool *)>("paramBoolean");

    // Prepare parameter values 
    bool value=true;

    // Calls binded ScriptEngine function with parameters
    paramBoolean(&value);

 	return 0;
}

