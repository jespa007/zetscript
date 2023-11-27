#include "zetscript.h"

using zetscript::ObjectObject;

int main()
{
	zetscript::ZetScript zs;

    // Evaluates ZetScript function 'returnObject' that returns 'Object' value
    zs.eval(
 		"function returnObject(){\n"
        "    return {a:1,b:true,c:\"String\"};\n"
        "}\n"
 	);

    // It binds 'returnObject' as 'ObjectObject *(void)'
    auto returnObject=zs.bindFunction<ObjectObject *()>("returnObject");

    // Calls ZetScript function which it returns 'ObjectObject *' reference
    auto object_object=returnObject();

    // Prints its value by console.
    printf("result : %s\n",object_object->toString().toConstChar());

    // 'unrefLifetimeObject' it decreases the reference count of the script object to tell is not used anymore
    zs.unrefLifetimeObject(object_object);

 	return 0;
}

