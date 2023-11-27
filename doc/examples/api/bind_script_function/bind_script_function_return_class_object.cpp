#include "zetscript.h"

using zetscript::DictionaryObject;

int main()
{
	zetscript::ZetScript zs;

    // Evaluates ZetScript class Number and function 'newNumber' that returns a new instance of type 'Number'
    zs.eval(
      "class Number{ \n"
      "  var value=0;\n"
      "}\n"
      "\n"
      "// 'returnNumber' instances 'Number' type\n"
      "\n"
      "function newNumber(){\n"
      "   return new Number();\n"
      "}\n"
 	);

    // It binds 'newNumber' as '(DictionaryObject *)(void)'
    auto newNumber=zs.bindFunction<DictionaryObject *()>("newNumber");

   // Calls ZetScript function which it returns 'DictionaryObject *' reference
    auto number=newNumber();

    // Prints return value by console.
    printf("result : %s\n",number->toString().toConstChar());

   // 'unrefLifetimeObject' it decreases the reference count of the script object to tell is not used anymore
   zs.unrefLifetimeObject(number);    

 	return 0;
}

