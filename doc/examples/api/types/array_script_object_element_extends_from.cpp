#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::ArrayScriptObject;
using zetscript::zs_int;
using zetscript::zs_float;
using zetscript::StringScriptObject;

// Check whether the array element is type integer, float or string
void printArrayElementTypes(ZetScript *_zs,ArrayScriptObject *_array_object){
	printf("Array element types : \n");
	for(int i=0; i < _array_object->length(); i++){
        printf("Position %i is type can extend from '",i);

        if(_array_object->elementInstanceOf<zs_int>(i)){
            printf(" Integer");
        }

        if(_array_object->elementInstanceOf<zs_float>(i)){
            printf(" Float");
        }
        
        if(_array_object->elementInstanceOf<StringScriptObject *>(i)){
            printf(" String");
        }

        printf("'\n");
	}
}

int main(){
	zetscript::ZetScript zs;

	// registers printString
	zs.registerFunction("printArrayElementTypes",printArrayElementTypes);

	// calls printArrayElementTypes
	zs.eval(
        "printArrayElementTypes(["
            "0"                 // Position 1: type integer
            ",10.5"             // Position 2: type float
            ",\"Hello World!\"" // Position 3: type string
        "])");
	
	return 0;
}