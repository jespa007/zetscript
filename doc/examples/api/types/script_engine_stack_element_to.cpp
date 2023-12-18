#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::StackElement;
using zetscript::zs_float;
using zetscript::ObjectScriptObject;

int main(){
	ScriptEngine script_engine;

	// Eval an expression that returns a 'Float' type as result
	StackElement result=script_engine.compileAndRun("return 10.5");

	// Prints conversion of stack element to zs_float
	printf("StackElement to 'zs_float' result : %0.2f\n",script_engine.stackElementTo<zs_float>(result));

	// Eval an expression that returns a 'Object' type as result
	result=script_engine.compileAndRun("return {f1:1,f2:2}");

	// Converts stack element to ObjectScriptObject
	ObjectScriptObject *result_object=script_engine.stackElementTo<ObjectScriptObject *>(result);

	auto keys=result_object->getKeys();

	printf("StackElement to 'ObjectScriptObject' result : {");

	// Prints ObjectScriptObject
	for(int i=0; i < keys.length(); i++){
		if(i > 0){
			printf(",");	
		}
		printf("'%s' : %0.2f",keys.get(i).toConstChar(), result_object->get<zs_int>(keys.get(i)));
	}

	printf("}\n");

	// unref lifetime object due is not needed anymore.
	script_engine.unrefLifetimeObject(result_object);
	
	return 0;
}