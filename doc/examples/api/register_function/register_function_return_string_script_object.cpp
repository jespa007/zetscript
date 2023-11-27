#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::StringObject;

// ZetScript C++ interface function
StringObject *returnString(ZetScript *_zs){

	// instance new StringObject using ZetScript context
	StringObject *string=_zs->newStringObject();

    // set string value "Hello world (StringObject)"
    string->set("Hello world (StringObject)");

    return string;
}

int main(){
	ZetScript zs;

	// Registers function interface
    zs.registerFunction("returnString",returnString);

    // Evaluates a script that calls the registered function and prints its result by console
    zs.eval(
        "Console::outln(\"result : \"+returnString());"
 	);

    return 0;
}
