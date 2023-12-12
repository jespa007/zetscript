#include "zetscript.h"

using zetscript::String;

int main(){
   zetscript::ScriptEngine script_engine;

   // Evaluates ScriptEngine function 'returnString' that returns 'String' value
   script_engine.compile(
 		"function returnString(){\n"
        "    return \"Hello World\";\n"
        "}\n"
 	);

   // It binds 'returnString' as 'String(void)'
   auto returnString=script_engine.bindScriptFunction<String()>("returnString");

   // Calls ScriptEngine function and prints return value by console.
   printf("result : %s\n",returnString().toConstChar());
 	return 0;
}