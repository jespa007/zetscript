#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::StringScriptObject;

// ZetScript C++ interface function
StringScriptObject *returnString(ZetScript *_zs){

	// instance new StringScriptObject using ZetScript context
	StringScriptObject *string=_zs->newStringScriptObject();

    // set string value "Hello world (StringScriptObject)"
    string->set("Hello world (StringScriptObject)");

    return string;
}

int main(){
	ZetScript zs;

	// Registers function interface
    zs.registerFunction("returnString",returnString);

    // Evaluates a script that calls the registered function and prints its result by console
    zs.compileAndRun(
        "Console::outln(\"result : \"+returnString());"
 	);

    return 0;
}
