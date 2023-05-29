#include "zetscript.h"

using zetscript::ArrayScriptObject;

int main()
{
	zetscript::ZetScript zs;

    // Evaluates ZetScript function 'returnString' that returns 'Array' value
    zs.eval(
 		"function returnArray(){\n"
        "    return [1,true,\"String\"];\n"
        "}\n"
 	);

    // It binds 'returnArray' as 'StringScriptObject *(void)'
    auto returnArray=zs.bindScriptFunction<ArrayScriptObject *()>("returnArray");
    auto array=returnArray();

    // Calls ZetScript function and prints return value by console.
    for(int i=0; i < array->length(); i++){
      printf("Array[%i]= : %s\n",array->toString().c_str());
    }

 	return 0;
}

