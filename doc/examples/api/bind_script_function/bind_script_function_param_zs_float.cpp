#include "zetscript.h"

int main(){
   zetscript::ScriptEngine script_engine;

   // Evaluates ZetScript function 'printAdd' that prints the result of the operation '+' from two arguments
   script_engine.compile(
 		"function printAdd(_value1,_value2){\n"
        "    Console::outln(\"result : \"+(_value1 + _value2));\n"
        "}\n"
 	);

  // It binds 'printAdd' as '(void *)(zs_float, zs_float *)'
  auto printAdd=script_engine.bindScriptFunction<
    void(
      zetscript::zs_float *_value1
      ,zetscript::zs_float *_value2
    )>("printAdd");

  // Prepare parameter values 
  zetscript::zs_float value1=3.5;
  zetscript::zs_float value2=10.7;

  // Calls binded ZetScript function with parameters
   printAdd(&value1,&value2);

 	return 0;
}

