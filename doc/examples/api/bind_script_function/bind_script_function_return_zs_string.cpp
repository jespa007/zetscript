#include "zetscript.h"

using zetscript::String;

int main(){
   zetscript::ZetScript zs;

   // Evaluates ZetScript function 'returnString' that returns 'String' value
   zs.compile(
 		"function returnString(){\n"
        "    return \"Hello World\";\n"
        "}\n"
 	);

   // It binds 'returnString' as 'String(void)'
   auto returnString=zs.bindScriptFunction<String()>("returnString");

   // Calls ZetScript function and prints return value by console.
   printf("result : %s\n",returnString().toConstChar());
 	return 0;
}