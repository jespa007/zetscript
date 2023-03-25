#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::ObjectScriptObject;

// c function expects an array of integers and floats
void funParamObjectScriptObject(ZetScript *_zs, ObjectScriptObject *_array){
    for(size_t i=0; i < _array->length(); i++){
        printf("Integer value %i. Float value: \n",_array->elementAt<zs_float>(i),_array->elementAt<zs_float>(i));
    }
}

int main(int argc, char *argv[]){
	ZetScript zs;

    zs.bindFunction("funParamObjectScriptObject",funParamObjectScriptObject);

    // call c function with string param
    zs.eval(
        "funParamObjectScriptObject({"
        "    a:0"
        "     ,b:1"
        "     ,c:2"
        "     ,d:3"
        "     ,e:4"

        "});"
 	);


 	return 0;
}

