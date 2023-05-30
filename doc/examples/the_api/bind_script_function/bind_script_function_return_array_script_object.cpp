#include "zetscript.h"

using zetscript::ArrayScriptObject;

int main()
{
	zetscript::ZetScript zs;

  // Evaluates ZetScript function 'returnString' that returns 'Array' value
  zs.eval(
  "function returnArray(){\n"
      "    return [1,true,\"String\"];\n"
      "}\n"
  );

  // It binds 'returnArray' as 'StringScriptObject *(void)'
  auto returnArray=zs.bindScriptFunction<ArrayScriptObject *()>("returnArray");

  // Calls ZetScript function which it returns 'ArrayScriptObject'
  auto array_object=returnArray();

  // Calls ZetScript function and prints return value by console.
  printf("result : %s\n",array_object->toString().c_str());

   // 'unrefLifetimeObject' it decreases the reference count of thr script object to tell is not used anymore
   zs.unrefLifetimeObject(array_object);

 	return 0;
}

