#include "zetscript.h"

using zetscript::String;

int main(){
   zetscript::ZetScript zs;

   // Evaluates ZetScript function 'returnString' that returns 'String' value
   zs.eval(
 		"function returnString(){\n"
        "    return \"Hello World\";\n"
        "}\n"
 	);

   // It binds 'returnString' as 'String(void)'
   auto returnString=zs.bindFunction<String()>("returnString");

   // Calls ZetScript function and prints return value by console.
   printf("result : %s\n",returnString().toConstChar());
 	return 0;
}