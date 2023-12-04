#include "zetscript.h"

using zetscript::ArrayScriptObject;

int main()
{
	zetscript::ZetScript zs;

  // Evaluates ZetScript function 'returnArray' that returns 'Array' value
  zs.eval(
  "function returnArray(){\n"
      "    return [1,true,\"String\"];\n"
      "}\n"
  );

  // It binds 'returnArray' as 'ArrayScriptObject *(void)'
  auto returnArray=zs.bindFunction<ArrayScriptObject *()>("returnArray");

  // Calls ZetScript function which it returns 'ArrayScriptObject *' reference
  auto array_object=returnArray();

  // Prints its value by console.
  printf("result : %s\n",array_object->toString().toConstChar());

   // 'unrefLifetimeObject' it decreases the reference count of the script object to tell is not used anymore
   zs.unrefLifetimeObject(array_object);

 	return 0;
}
