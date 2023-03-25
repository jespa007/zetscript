#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::ArrayScriptObject;
using zetscript::zs_int;
using zetscript::zs_float;

// c function expects an array of integers and floats
void funParamArrayScriptObject(ZetScript *_zs, ArrayScriptObject *_array){
	printf("Values in array:\n");
    for(int i=0; i < _array->length(); i++){
    	zs_int v=_array->get(i);
        printf(
        		"value: %i\n"
				,(int)_array->get<zs_int>(i)
		);
    }
}

int main(int argc, char *argv[]){
	ZetScript zs;

    zs.bindFunction("funParamArrayScriptObject",funParamArrayScriptObject);

    // call c function with string param
    zs.eval(
        "funParamArrayScriptObject(["
        "    0,1,2,3,4,5"
        "]);"
 	);


 	return 0;
}

