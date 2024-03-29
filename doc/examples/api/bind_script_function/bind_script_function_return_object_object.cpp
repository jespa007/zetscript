#include "zetscript.h"

int main()
{
	zetscript::ScriptEngine script_engine;

    // Evaluates ZetScript function 'returnObject' that returns 'ScriptObject' value
    script_engine.compile(
 		"function returnObject(){\n"
        "    return {a:1,b:true,c:\"String\"};\n"
        "}\n"
 	);

   // It binds 'returnObject' as 'ObjectScriptObject *(void)'
   auto returnObject=script_engine.bindScriptFunction<
      zetscript::ObjectScriptObject *()
   >("returnObject");

   // Calls ZetScript function which it returns 'ObjectScriptObject *' reference
   auto object_object=returnObject();

   // Prints its value by console.
   printf("result : %s\n",object_object->toString().toConstChar());

   // 'unrefLifetimeObject' it decreases the reference count of the script object to tell is not used anymore
   script_engine.unrefLifetimeObject(object_object);

 	return 0;
}

