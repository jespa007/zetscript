#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::ArrayScriptObject;


void checkLengthArray(ScriptEngine *_zs,ArrayScriptObject *_array_object){
	// instance new ArrayScriptObject using ScriptEngine context
    printf("The length of array is : %i\n",_array_object->length());
}

int main(){
	zetscript::ScriptEngine zs;

	// registers modifyArray
	zs.registerFunction("checkLengthArray",checkLengthArray);

	// create a initialized array, it prints. then prints the 
	// the array after call 'modifyArray'
	zs.compileAndRun(
		"var v=[0,1.5,\"world\"]\n"
		"checkLengthArray(v)\n"
	);
	
	return 0;
}