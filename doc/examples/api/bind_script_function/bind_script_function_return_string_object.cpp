#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::StringScriptObject;

int main()
{
  ScriptEngine se;

  // Evaluates ScriptEngine function 'returnString' that returns 'String' value
  se.compile(
  "function returnString(){\n"
      "    return \"Hello World\";\n"
      "}\n"
 	);

  // It binds 'returnString' as 'StringScriptObject *(void)'
  auto returnString=se.bindScriptFunction<StringScriptObject *()>("returnString");

  // Calls ScriptEngine function which it returns 'StringScriptObject *' reference
  auto object=returnString();

  // Prints its value by console.
  printf("result : %s\n",object->get().toConstChar());

  // 'unrefLifetimeObject' it decreases the reference count of the script object to tell is not used anymore
  se.unrefLifetimeObject(object);

 	return 0;
}

