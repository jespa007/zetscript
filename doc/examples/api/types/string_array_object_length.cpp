#include "zetscript.h"

void checkLengthArray(
	zetscript::ScriptEngine *_script_engine
	,zetscript::ArrayScriptObject *_array_object
){
	// instance new ArrayScriptObject using ScriptEngine instance
    printf("The length of array is : %i\n",_array_object->length());
}

int main(){
	zetscript::ScriptEngine script_engine;

	// registers modifyArray
	script_engine.registerFunction("checkLengthArray",checkLengthArray);

	// create a initialized array, it prints. then prints the 
	// the array after call 'modifyArray'
	script_engine.compileAndRun(
		"var v=[0,1.5,\"world\"]\n"
		"checkLengthArray(v)\n"
	);
	
	return 0;
}