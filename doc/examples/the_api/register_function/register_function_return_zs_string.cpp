#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::zs_string;

zs_string returnString(ZetScript *_zs){
    return "Hello world (zs_string)";
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

