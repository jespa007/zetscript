#include "zetscript.h"

int main()
{
	zetscript::ZetScript zs;

    // Evaluates ZetScript function 'paramBoolean' that prints the contents of '_value'
    zs.eval(
 		"function paramBoolean(_value){\n"
        "    Console::outln(\"result : \"+_value);\n"
        "}\n"
 	);

    // It binds 'paramBoolean' as '(void *)(bool *)'
    auto paramBoolean=zs.bindFunction<void(bool *)>("paramBoolean");

    // Prepare parameter values 
    bool value=true;

    // Calls binded ZetScript function with parameters
    paramBoolean(&value);

 	return 0;
}

