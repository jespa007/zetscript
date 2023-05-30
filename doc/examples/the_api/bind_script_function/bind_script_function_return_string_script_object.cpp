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

    // Calls ZetScript function which it returns 'StringScriptObject'
    auto script_object=returnString();

    // Prints its value by console.
    printf("result : %s\n",script_object->get().c_str());

   // Decreases the reference count of script object
   zs.unrefLifetimeObject(script_object);

 	return 0;
}

