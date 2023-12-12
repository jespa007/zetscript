#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::StringScriptObject;

int main()
{
  ScriptEngine zs;

  // Evaluates ScriptEngine function 'returnString' that returns 'String' value
  zs.compile(
  "function returnString(){\n"
      "    return \"Hello World\";\n"
      "}\n"
 	);

  // It binds 'returnString' as 'StringScriptObject *(void)'
  auto returnString=zs.bindScriptFunction<StringScriptObject *()>("returnString");

  // Calls ScriptEngine function which it returns 'StringScriptObject *' reference
  auto object=returnString();

  // Prints its value by console.
  printf("result : %s\n",object->get().toConstChar());

  // 'unrefLifetimeObject' it decreases the reference count of the script object to tell is not used anymore
  zs.unrefLifetimeObject(object);

 	return 0;
}

