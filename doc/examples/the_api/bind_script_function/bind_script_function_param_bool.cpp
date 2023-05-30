#include "zetscript.h"

int main()
{
	zetscript::ZetScript zs;

    // Evaluates ZetScript function 'paramBoolean' that accepts 'Boolean' value as parameter
    zs.eval(
 		"function paramBoolean(_bool){\n"
        "    Console::outln(\"result : \"+_bool);\n"
        "}\n"
 	);

    // It binds 'returnBoolean' as '(void *)(bool *)'
    auto paramBoolean=zs.bindScriptFunction<void(bool *)>("returnBoolean");

    // Prepare parameter values 
    bool value=true;

    // Calls ZetScript function
    paramBoolean(&value);

 	return 0;
}

