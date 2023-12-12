#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::ArrayScriptObject;
using zetscript::zs_int;
using zetscript::zs_float;
using zetscript::String;


// Print contents of an array.
void printArray(ScriptEngine *_se,ArrayScriptObject *_array_object){
	printf("Array contents : [");
	for(int i=0; i < _array_object->length(); i++){
		if(i>0){
			printf(",");
		}
		if(_array_object->elementInstanceOf<zs_int>(i)){
			printf("%i",(int)_array_object->get<zs_int>(i));
		}else if(_array_object->elementInstanceOf<zs_float>(i)){
			printf("%f",_array_object->get<zs_float>(i));
		}else if(_array_object->elementInstanceOf<String>(i)){
			printf("'%s'",_array_object->get<String>(i).toConstChar());
		}else{
			printf("N/A");
		}
	}
	printf("]\n");
}

int main(){
	zetscript::ScriptEngine se;

	// registers printString
	se.registerFunction("printArray",printArray);

	// calls printArray
	se.compileAndRun("printArray([0,10.5,\"Hello World\"])");
	
	return 0;
}