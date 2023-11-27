#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::StringObject;

int main()
{
  ZetScript zs;

  // Evaluates ZetScript function 'returnString' that returns 'String' value
  zs.eval(
  "function returnString(){\n"
      "    return \"Hello World\";\n"
      "}\n"
 	);

  // It binds 'returnString' as 'StringObject *(void)'
  auto returnString=zs.bindFunction<StringObject *()>("returnString");

  // Calls ZetScript function which it returns 'StringObject *' reference
  auto object=returnString();

  // Prints its value by console.
  printf("result : %s\n",object->get().toConstChar());

  // 'unrefLifetimeObject' it decreases the reference count of the script object to tell is not used anymore
  zs.unrefLifetimeObject(object);

 	return 0;
}

