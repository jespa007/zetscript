#include "zetscript.h"

using zetscript::ObjectScriptObject;

int main()
{
	zetscript::ZetScript zs;

    // Evaluates ZetScript function 'returnObject' that returns 'Object' value
    zs.eval(
 		"function returnObject(){\n"
        "    return {a:1,b:true,c:\"String\"};\n"
        "}\n"
 	);

    // It binds 'returnObject' as 'ObjectScriptObject *(void)'
    auto returnObject=zs.bindScriptFunction<ObjectScriptObject *()>("returnArray");
    auto object=returnObject();

    // Calls ZetScript function and prints return value by console.
    for(int i=0; i < array->length(); i++){
      printf("Array[%i]= : %s\n",array->toString().c_str());
    }

 	return 0;
}

