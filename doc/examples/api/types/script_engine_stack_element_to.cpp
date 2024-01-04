#include "zetscript.h"

int main(){
	zetscript::ScriptEngine script_engine;

    // Returns Object in StackElement as a result of compile and run "return {f1:10,f2:20,f4:40}"
	zetscript::StackElement result=script_engine.compileAndRun("return {f1:10,f2:20,f4:40}");

	// Converts stack element to ObjectScriptObject
	zetscript::ObjectScriptObject *result_object=script_engine.stackElementTo<zetscript::ObjectScriptObject *>(result);

	// ...
	// perform some operations with 'result_object'
	// ...

	// unref lifetime object due is not needed anymore.
	script_engine.unrefLifetimeObject(result_object);
	
	return 0;
}