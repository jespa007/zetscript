#include "zetscript.h"

using zetscript::String;

int main(){
   zetscript::ScriptEngine se;

   // Evaluates ScriptEngine function 'returnString' that returns 'String' value
   se.compile(
 		"function returnString(){\n"
        "    return \"Hello World\";\n"
        "}\n"
 	);

   // It binds 'returnString' as 'String(void)'
   auto returnString=se.bindScriptFunction<String()>("returnString");

   // Calls ScriptEngine function and prints return value by console.
   printf("result : %s\n",returnString().toConstChar());
 	return 0;
}