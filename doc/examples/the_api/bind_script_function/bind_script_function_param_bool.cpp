#include "zetscript.h"

int main()
{
	zetscript::ZetScript zs;

    // Evaluates ZetScript function 'concat' that prints the contents of '_value'
    zs.eval(
 		"function paramBoolean(_value){\n"
        "    Console::outln(\"result : \"+_value);\n"
        "}\n"
 	);

    // It binds 'returnBoolean' as '(void *)(bool *)'
    auto paramBoolean=zs.bindScriptFunction<void(bool *)>("paramBoolean");

    // Prepare parameter values 
    bool value=true;

    // Calls ZetScript function
    paramBoolean(&value);

 	return 0;
}

