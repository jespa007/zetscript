#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::ArrayScriptObject;

// c function expects an array of integers and floats
void funParamArrayScriptObject(ZetScript *_zs, ArrayScriptObject *_array){
    for(size_t i=0; i < _array->length(); i++){
        printf("Integer value %i. Float value: \n",_array->elementAt<zs_float>(i),_array->elementAt<zs_float>(i));
    }
}

int main(int argc, char *argv[]){
	ZetScript zs;

    zs.bindFunction("funParamArrayScriptObject",funParamArrayScriptObject);

    // call c function with string param
    zs.eval(
        "funParamArrayScriptObject(["
        "    0,1,2,3,4,4.5"
        "]);"
 	);


 	return 0;
}

