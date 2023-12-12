#include "zetscript.h"

using zetscript::zs_int;

int main(){
	zetscript::ScriptEngine script_engine;

    // Evaluates ScriptEngine function 'returnInteger' that returns 'Integer' value
    script_engine.compile(
 		"function returnInteger(){\n"
        "    return 10;\n"
        "}\n"
 	);

   // It binds 'returnInteger' as 'zs_int(void)'
   auto returnInteger=script_engine.bindScriptFunction<zs_int()>("returnInteger");

   // Calls ScriptEngine function and prints return value by console.
   printf("result : %ld\n",returnInteger());
 	return 0;
}