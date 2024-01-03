#include "zetscript.h"

void printArrayLength(
	zetscript::ScriptEngine *_script_engine
	,zetscript::ArrayScriptObject *_array_object
){
	printf("The length of vector is %i\n",_array_object->length());
}

int main(){
	zetscript::ScriptEngine script_engine;

	// registers printArrayLength
	script_engine.registerFunction("printArrayLength",printArrayLength);

	// calls printArrayLength
	script_engine.compileAndRun("printArrayLength([0,1,2,3])");
	
	return 0;
}