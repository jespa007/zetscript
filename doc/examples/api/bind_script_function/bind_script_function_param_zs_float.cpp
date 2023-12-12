#include "zetscript.h"

using zetscript::zs_float;

int main(){
   zetscript::ScriptEngine zs;

   // Evaluates ScriptEngine function 'printAdd' that prints the result of the operation '+' from two arguments
   zs.compile(
 		"function printAdd(_value1,_value2){\n"
        "    Console::outln(\"result : \"+(_value1 + _value2));\n"
        "}\n"
 	);

   // It binds 'printAdd' as '(void *)(zs_float, zs_float *)'
   auto printAdd=zs.bindScriptFunction<void(zs_float *_value1, zs_float *_value2)>("printAdd");

   // Prepare parameter values 
   zs_float value1=3.5;
   zs_float value2=10.7;

   // Calls binded ScriptEngine function with parameters
   printAdd(&value1,&value2);

 	return 0;
}

