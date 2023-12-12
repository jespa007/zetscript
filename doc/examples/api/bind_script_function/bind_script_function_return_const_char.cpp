#include "zetscript.h"

int main()
{
	zetscript::ScriptEngine se;

    // Evaluates ScriptEngine function 'returnString' that returns 'String' value
    se.compile(
 		"function returnString(){\n"
        "    return \"Hello World\";\n"
        "}\n"
 	);

    // It binds 'returnString' as 'const char *(void)'
    auto returnString=se.bindScriptFunction<const char *()>("returnString");

    // Calls ScriptEngine function and prints return value by console.
    printf("result : %s\n",returnString());
    
 	return 0;
}

