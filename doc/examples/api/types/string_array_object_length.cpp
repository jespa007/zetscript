#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::ArrayScriptObject;


void checkLengthArray(ScriptEngine *_se,ArrayScriptObject *_array_object){
	// instance new ArrayScriptObject using ScriptEngine context
    printf("The length of array is : %i\n",_array_object->length());
}

int main(){
	zetscript::ScriptEngine se;

	// registers modifyArray
	se.registerFunction("checkLengthArray",checkLengthArray);

	// create a initialized array, it prints. then prints the 
	// the array after call 'modifyArray'
	se.compileAndRun(
		"var v=[0,1.5,\"world\"]\n"
		"checkLengthArray(v)\n"
	);
	
	return 0;
}