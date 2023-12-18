#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::ObjectScriptObject;

int main(){
	ScriptEngine script_engine;

    // Eval an expression that returns a 'Object' type as result
	auto result=script_engine.compileAndRun("return {f1:10,f2:20,f4:40}");

	// Converts stack element to ObjectScriptObject
	ObjectScriptObject *result_object=script_engine.stackElementTo<ObjectScriptObject *>(result);

	// ...
	// perform some operations with 'result_object'
	// ...

	// unref lifetime object due is not needed anymore.
	script_engine.unrefLifetimeObject(result_object);
	
	return 0;
}