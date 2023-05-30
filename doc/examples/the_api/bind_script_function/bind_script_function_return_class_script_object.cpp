#include "zetscript.h"

using zetscript::ObjectScriptObject;

int main()
{
	zetscript::ZetScript zs;

    // Evaluates ZetScript class A and function 'returnNewA' that returns an instance of type 'A'
    zs.eval(
      "class A{ \n"
      "  var a=10;\n"
      "  var b=true;\n"
      "  var c=\"String\";\n"
      "}\n"
      "\n"
      "// 'returnNewA' instances A type\n"
      "\n"
      "function returnNewA(){\n"
      "   return new A();\n"
      "}\n"
 	);

    // It binds 'returnNewA' as '(ObjectScriptObject *)(void)'
    auto returnNewA=zs.bindScriptFunction<ObjectScriptObject *()>("returnNewA");

   // Calls ZetScript function which it returns 'ObjectScriptObject *' reference
    auto a=returnNewA();

    // Prints return value by console.
    printf("result : %s\n",a->toString().c_str());

   // 'unrefLifetimeObject' it decreases the reference count of thr script object to tell is not used anymore
   zs.unrefLifetimeObject(a);    

 	return 0;
}

