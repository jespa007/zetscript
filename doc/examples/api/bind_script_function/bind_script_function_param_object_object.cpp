#include "zetscript.h"

using zetscript::DictionaryScriptObject;
using zetscript::zs_int;
using zetscript::zs_float;
using zetscript::String;

int main()
{
	zetscript::ZetScript zs;

    // Evaluates ZetScript function 'printConcat' that prints the result of the operation '+' from two arguments
    zs.eval(
 		"function printConcat(_value1, _value2){\n"
        "    Console::outln(\"result : \"+(_value1+_value2));\n"
        "}\n"
 	);

    // It binds 'printConcat' as '(void *)(ArrayScriptObject *, ArrayScriptObject *)'
    auto printConcat=zs.bindFunction<void(DictionaryScriptObject * _value1, DictionaryScriptObject * _value2)>("printConcat");

    // Prepare param values
    auto object1=zs.newDictionaryScriptObject();
    auto object2=zs.newDictionaryScriptObject();
    
    // push values for object1
    object1->set<bool>("a",true);
    object1->set<zs_int>("b",10);
    object1->set<const char *>("c","Hello");
    
    // push values for object2
    object2->set<bool>("d",false);
    object2->set<zs_float>("e",20.5);
    object2->set<const char *>("f","World");


    // Calls binded ZetScript function with parameters
    printConcat(object1,object2);


 	return 0;
}

