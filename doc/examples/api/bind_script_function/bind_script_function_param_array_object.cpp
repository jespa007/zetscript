#include "zetscript.h"

int main()
{
	zetscript::ScriptEngine script_engine;

    // Evaluates ZetScript function 'printConcat' that prints the result of the operation '+' from two arguments
    script_engine.compile(
 		"function printConcat(_value1, _value2){\n"
        "    Console::outln(\"result : \"+(_value1+_value2));\n"
        "}\n"
 	);

    // It binds 'printConcat' as '(void *)(ArrayScriptObject *, ArrayScriptObject *)'
    auto printConcat=script_engine.bindScriptFunction<
      void(
         zetscript::ArrayScriptObject * _value1
         ,zetscript::ArrayScriptObject * _value2
      )>("printConcat");

    // Prepare param values
    auto array1=script_engine.newArrayScriptObject();
    auto array2=script_engine.newArrayScriptObject();

   // push values for array1
   array1->push<bool>(false);
   array1->push<zetscript::zs_int>(10);
   array1->push<const char *>("Hello");

   // push values for array2
   array2->push<bool>(true);
   array2->push<zetscript::zs_float>(20.5);
   array2->push<const char *>("World");


    // Calls binded ZetScript function with parameters
    printConcat(array1,array2);


 	return 0;
}