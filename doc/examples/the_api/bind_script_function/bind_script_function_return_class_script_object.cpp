#include "zetscript.h"

using zetscript::ObjectScriptObject;

int main()
{
	zetscript::ZetScript zs;

    // Evaluates ZetScript class Point and function 'returnNewPoint' that returns an instance of type 'Point'
    zs.eval(
      "class Point{ \n"
      "  var x=10;\n"
      "  var y=20;\n"
      "}\n"
      "\n"
      "// 'returnPoint' instances 'Point' type\n"
      "\n"
      "function returnPoint(){\n"
      "   return new Point();\n"
      "}\n"
 	);

    // It binds 'returnNewPoint' as '(ObjectScriptObject *)(void)'
    auto returnPoint=zs.bindScriptFunction<ObjectScriptObject *()>("returnPoint");

   // Calls ZetScript function which it returns 'ObjectScriptObject *' reference
    auto point=returnPoint();

    // Prints return value by console.
    printf("result : %s\n",point->toString().c_str());

   // 'unrefLifetimeObject' it decreases the reference count of thr script object to tell is not used anymore
   zs.unrefLifetimeObject(point);    

 	return 0;
}

