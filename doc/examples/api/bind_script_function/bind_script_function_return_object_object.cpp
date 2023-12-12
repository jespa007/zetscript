#include "zetscript.h"

using zetscript::ObjectScriptObject;

int main()
{
	zetscript::ScriptEngine zs;

    // Evaluates ScriptEngine function 'returnObject' that returns 'ScriptObject' value
    zs.compile(
 		"function returnObject(){\n"
        "    return {a:1,b:true,c:\"String\"};\n"
        "}\n"
 	);

    // It binds 'returnObject' as 'ObjectScriptObject *(void)'
    auto returnObject=zs.bindScriptFunction<ObjectScriptObject *()>("returnObject");

    // Calls ScriptEngine function which it returns 'ObjectScriptObject *' reference
    auto object_object=returnObject();

    // Prints its value by console.
    printf("result : %s\n",object_object->toString().toConstChar());

    // 'unrefLifetimeObject' it decreases the reference count of the script object to tell is not used anymore
    zs.unrefLifetimeObject(object_object);

 	return 0;
}

