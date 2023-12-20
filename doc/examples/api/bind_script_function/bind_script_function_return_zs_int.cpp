#include "zetscript.h"

using zetscript::zs_int;

int main(){
	zetscript::ScriptEngine script_engine;

    // Evaluates ZetScript function 'returnInteger' that returns 'Integer' value
    script_engine.compile(
 		"function returnInteger(){\n"
        "    return 10;\n"
        "}\n"
 	);

   // It binds 'returnInteger' as 'zs_int(void)'
   auto returnInteger=script_engine.bindScriptFunction<zs_int()>("returnInteger");

   // Calls ZetScript function and prints return value by console.
   printf("result : %ld\n",(long int)returnInteger());
 	return 0;
}