#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::StackElement;
using zetscript::zs_int;
using zetscript::ArrayScriptObject;

int main(){
	ScriptEngine script_engine;

	// Compiles script function 'returnNewArray' that returns a 'Object' type as result and it leaves a reference
	script_engine.compile("function returnNewArray(){\n"
		"return [0,1,2,3];\n"
	"}\n");

	auto returnNewArray=script_engine.bindScriptFunction<ArrayScriptObject *()>("returnNewArray");

	auto array_object = returnNewArray();

	printf("array_object : [");

	// Prints ObjectScriptObject
	for(int i=0; i < array_object->length(); i++){
		if(i > 0){
			printf(",");	
		}
		printf("%i",array_object->get<zs_int>(i));
	}

	printf("]\n");

	// unref lifetime array object due is not needed anymore.
	script_engine.unrefLifetimeObject(array_object);

	return 0;
}