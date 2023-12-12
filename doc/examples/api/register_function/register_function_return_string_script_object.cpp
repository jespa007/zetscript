#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::StringScriptObject;

// ScriptEngine C++ interface function
StringScriptObject *returnString(ScriptEngine *_se){

	// instance new StringScriptObject using ScriptEngine context
	StringScriptObject *string=_se->newStringScriptObject();

    // set string value "Hello world (StringScriptObject)"
    string->set("Hello world (StringScriptObject)");

    return string;
}

int main(){
	ScriptEngine se;

	// Registers function interface
    se.registerFunction("returnString",returnString);

    // Evaluates a script that calls the registered function and prints its result by console
    se.compileAndRun(
        "Console::outln(\"result : \"+returnString());"
 	);

    return 0;
}
