#include "zetscript.h"

using zetscript::ObjectScriptObject;

int main()
{
	zetscript::ZetScript zs;

    // Evaluates ZetScript function 'returnNewA' that returns 'Boolean' value
    zs.eval(
      "class A{ \n"
      "  var a=10;\n"
      "  var b=true;\n"
      "  var c=\"String\";\n"
      "}\n"
      "function returnNewA(){\n"
      "   return new A();\n"
      "}\n"
 	);

    // It binds 'returnNewA' as '(ObjectScriptObject *)(void)'
    auto returnNewA=zs.bindScriptFunction<ObjectScriptObject *()>("returnNewA");

    auto a=returnNewA();

    // Prints return value by console.
    printf("result : %s\n",a->toString().c_str());
    

 	return 0;
}

