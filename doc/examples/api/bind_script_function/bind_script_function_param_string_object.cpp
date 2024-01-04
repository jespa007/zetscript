#include "zetscript.h"

int main(){

	zetscript::ScriptEngine script_engine;

    // Evaluates ZetScript function 'printConcat' that prints the result of the operation '+' from two arguments
    script_engine.compile(
 		"function printConcat(_value1, _value2){\n"
        "    Console::outln(\"result : \"+_value1+\" \"+_value2);\n"
        "}\n"
 	);

    // It binds 'printConcat' as '(void *)(StringScriptObject *, StringScriptObject *)'
   auto printConcat=script_engine.bindScriptFunction<
      void(
         zetscript::StringScriptObject * _value1
         ,zetscript::StringScriptObject * _value2
      )
   >("printConcat");

    // Prepare param values
    auto value1=script_engine.newStringScriptObject("Hello");
    auto value2=script_engine.newStringScriptObject("World");

    // Calls binded ZetScript function with parameters
    printConcat(value1,value2);


 	return 0;
}

