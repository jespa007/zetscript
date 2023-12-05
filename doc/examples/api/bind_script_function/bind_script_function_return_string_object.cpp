#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::StringScriptObject;

int main()
{
  ZetScript zs;

  // Evaluates ZetScript function 'returnString' that returns 'String' value
  zs.compile(
  "function returnString(){\n"
      "    return \"Hello World\";\n"
      "}\n"
 	);

  // It binds 'returnString' as 'StringScriptObject *(void)'
  auto returnString=zs.bindScriptFunction<StringScriptObject *()>("returnString");

  // Calls ZetScript function which it returns 'StringScriptObject *' reference
  auto object=returnString();

  // Prints its value by console.
  printf("result : %s\n",object->get().toConstChar());

  // 'unrefLifetimeObject' it decreases the reference count of the script object to tell is not used anymore
  zs.unrefLifetimeObject(object);

 	return 0;
}

