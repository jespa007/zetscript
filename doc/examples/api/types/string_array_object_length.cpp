#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::ArrayScriptObject;


void checkLengthArray(ZetScript *_zs,ArrayScriptObject *_array_object){
	// instance new ArrayScriptObject using ZetScript context
    printf("The length of array is : %i\n",_array_object->length());
}

int main(){
	zetscript::ZetScript zs;

	// registers modifyArray
	zs.registerFunction("checkLengthArray",checkLengthArray);

	// create a initialized array, it prints. then prints the 
	// the array after call 'modifyArray'
	zs.eval(
		"var v=[0,1.5,\"world\"]\n"
		"checkLengthArray(v)\n"
	);
	
	return 0;
}