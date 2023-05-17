#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::zs_int;

// ZetScript C++ interface function
zs_int returnInteger(ZetScript *_zs){
    return 10;
}

int main(){
	ZetScript zs;

	// Registers function interface
    zs.registerFunction("returnInteger",returnInteger);

    // Evaluates a script that calls the registered function and prints its result by console
    zs.eval(
        "Console::outln(\"result : \"+returnInteger());"
 	);

    return 0;
}

