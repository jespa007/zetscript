#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::ArrayScriptObject;
using zetscript::zs_int;
using zetscript::zs_float;

// c function expects an array of integers
void paramArrayScriptObject(ZetScript *_zs, ArrayScriptObject *_array){
	printf("Values in array => ");
    for(int i=0; i < _array->length(); i++){
        printf(" %i",(int)_array->get<zs_int>(i));
    }
    printf("\n");
}

int main(){
	ZetScript zs;

    zs.registerFunction("paramArrayScriptObject",paramArrayScriptObject);

    // call c function with string param
    zs.eval(
        "paramArrayScriptObject(["
        "    0,1,2,3,4,5"
        "]);"
 	);

 	return 0;
}