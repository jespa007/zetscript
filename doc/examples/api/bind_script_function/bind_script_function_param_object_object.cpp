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
         zetscript::ObjectScriptObject * _value1
         ,zetscript::ObjectScriptObject * _value2
   )>("printConcat");

    // Prepare param values
    auto object1=script_engine.newObjectScriptObject();
    auto object2=script_engine.newObjectScriptObject();
    
    // push values for object1
    object1->set<bool>("a",true);
    object1->set<zetscript::zs_int>("b",10);
    object1->set<const char *>("c","Hello");
    
    // push values for object2
    object2->set<bool>("d",false);
    object2->set<zetscript::zs_float>("e",20.5);
    object2->set<const char *>("f","World");


    // Calls binded ZetScript function with parameters
    printConcat(object1,object2);


 	return 0;
}

