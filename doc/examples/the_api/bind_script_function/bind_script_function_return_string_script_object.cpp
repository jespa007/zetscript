#include "zetscript.h"

using zetscript::StringScriptObject;

int main()
{
	zetscript::ZetScript zs;

    // Evaluates ZetScript function 'returnString' that returns 'String' value
    zs.eval(
 		"function returnString(){\n"
        "    return \"Hello World\";\n"
        "}\n"
 	);

    // It binds 'returnString' as 'StringScriptObject *(void)'
    auto returnString=zs.bindScriptFunction<StringScriptObject *()>("returnString");

    // Calls ZetScript function and prints return value by console.
    printf("result : %s\n",returnString()->get().c_str());

 	return 0;
}

