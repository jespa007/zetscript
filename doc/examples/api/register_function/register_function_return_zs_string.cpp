#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::String;

String returnString(ZetScript *_zs){
    return "Hello world (String)";
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

