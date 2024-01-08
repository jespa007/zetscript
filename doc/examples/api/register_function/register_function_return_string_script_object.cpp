#include "zetscript.h"

// ScriptEngine C++ interface function
zetscript::StringScriptObject *returnString(
    zetscript::ScriptEngine *_script_engine
){

	// instance new StringScriptObject using ScriptEngine instance
	zetscript::StringScriptObject *string=_script_engine->newStringScriptObject();

    // set string value "Hello world (StringScriptObject)"
    string->set("Hello world (StringScriptObject)");

    return string;
}

int main(){
	zetscript::ScriptEngine script_engine;

	// Registers the C function
    script_engine.registerFunction("returnString",returnString);

    // // Compiles and runs a script
    script_engine.compileAndRun(
        "Console::outln(\"result : \"+returnString());"
 	);

    return 0;
}
