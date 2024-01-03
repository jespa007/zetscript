#include "zetscript.h"

// Print contents of an array.
void printArray(
	zetscript::ScriptEngine *_script_engine
	,zetscript::ArrayScriptObject *_array_object
){
	printf("Array contents : [");
	for(int i=0; i < _array_object->length(); i++){
		if(i>0){
			printf(",");
		}
		if(_array_object->elementInstanceOf<zetscript::zs_int>(i)){
			printf("%i",(int)_array_object->get<zetscript::zs_int>(i));
		}else if(_array_object->elementInstanceOf<zetscript::zs_float>(i)){
			printf("%f",_array_object->get<zetscript::zs_float>(i));
		}else if(_array_object->elementInstanceOf<zetscript::String>(i)){
			printf("'%s'",_array_object->get<zetscript::String>(i).toConstChar());
		}else{
			printf("N/A");
		}
	}
	printf("]\n");
}

int main(){
	zetscript::ScriptEngine script_engine;

	// registers 'printArray' function
	script_engine.registerFunction("printArray",printArray);

	// calls printArray
	script_engine.compileAndRun("printArray([0,10.5,\"Hello World\"])");
	
	return 0;
}