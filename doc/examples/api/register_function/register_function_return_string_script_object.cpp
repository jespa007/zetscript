#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::StringScriptObject;

// ScriptEngine C++ interface function
StringScriptObject *returnString(ScriptEngine *_zs){

	// instance new StringScriptObject using ScriptEngine context
	StringScriptObject *string=_zs->newStringScriptObject();

    // set string value "Hello world (StringScriptObject)"
    string->set("Hello world (StringScriptObject)");

    return string;
}

int main(){
	ScriptEngine zs;

	// Registers function interface
    zs.registerFunction("returnString",returnString);

    // Evaluates a script that calls the registered function and prints its result by console
    zs.compileAndRun(
        "Console::outln(\"result : \"+returnString());"
 	);

    return 0;
}
