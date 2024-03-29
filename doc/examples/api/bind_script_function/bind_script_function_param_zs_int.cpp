#include "zetscript.h"

int main(){
	zetscript::ScriptEngine script_engine;

    // Evaluates ZetScript function 'add' that prints the result of the operation '+' from two arguments
    script_engine.compile(
 		"function printAdd(_value1,_value2){\n"
        "    Console::outln(\"result : \"+(_value1 + _value2));\n"
        "}\n"
 	);

   // It binds 'printAdd' as '(void *)(zs_int, zs_int)' (by value)
   auto printAddByValue=script_engine.bindScriptFunction<
      void(
         zetscript::zs_int _value1
         ,zetscript::zs_int _value2
      )
   >("printAdd");

    // Calls binded ZetScript function with parameters by value
    printAddByValue(10,10);

 	return 0;
}