#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::ArrayScriptObject;
using zetscript::zs_int;
using zetscript::zs_float;
using zetscript::zs_string;

// print array contents
void printArray(ZetScript *_zs,ArrayScriptObject *_array_object){
	printf("Array contents : %s",_array_object->toString().c_str());
}

int main(){
	zetscript::ZetScript zs;

	// registers printArray
	zs.registerFunction("printArray",printArray);

	// calls printArray
	zs.eval(
        "printArray(["
            "0"                 
            ",10.5"             
            ",\"Hello World!\"" 
        "])");
	
	return 0;
}