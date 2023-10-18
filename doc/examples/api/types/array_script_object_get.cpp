#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::ArrayScriptObject;
using zetscript::zs_int;
using zetscript::zs_float;
using zetscript::StringScriptObject;

// Print an array of integers.
void printArrayOfIntegers(ZetScript *_zs,ArrayScriptObject *_array_object){
	printf("Content vector : [");
	for(int i=0; i < _array_object->length(); i++){
		if(i>0){
			printf(",");
		}
		printf("%i",(int)_array_object->get<zs_int>(i));
	}
	printf("]\n");
}

int main(){
	zetscript::ZetScript zs;

	// registers printString
	zs.registerFunction("printArrayOfIntegers",printArrayOfIntegers);

	// calls printArray
	zs.eval("printArrayOfIntegers([0,10,20])");
	
	return 0;
}