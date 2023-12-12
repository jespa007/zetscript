#include "zetscript.h"

int main()
{
	zetscript::ScriptEngine script_engine;

    // Evaluates ScriptEngine function 'paramBoolean' that prints the contents of '_value'
    script_engine.compile(
 		"function paramBoolean(_value){\n"
        "    Console::outln(\"result : \"+_value);\n"
        "}\n"
 	);

    // It binds 'paramBoolean' as '(void *)(bool *)'
    auto paramBoolean=script_engine.bindScriptFunction<void(bool *)>("paramBoolean");

    // Prepare parameter values 
    bool value=true;

    // Calls binded ScriptEngine function with parameters
    paramBoolean(&value);

 	return 0;
}

