#include "zetscript.h"

int main(){

  zetscript::ScriptEngine script_engine;

  // Evaluates ZetScript function 'returnString' that returns 'String' value
  script_engine.compile(
  "function returnString(){\n"
      "    return \"Hello World\";\n"
      "}\n"
 	);

  // It binds 'returnString' as 'StringScriptObject *(void)'
  auto returnString=script_engine.bindScriptFunction<
    zetscript::StringScriptObject *()
  >("returnString");

  // Calls ZetScript function which it returns 'StringScriptObject *' reference
  auto object=returnString();

  // Prints its value by console.
  printf("result : %s\n",object->get().toConstChar());

  // 'unrefLifetimeObject' it decreases the reference count of the script object to tell is not used anymore
  script_engine.unrefLifetimeObject(object);

 	return 0;
}

