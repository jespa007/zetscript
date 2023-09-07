#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::zs_float;

zs_float returnFloat(ZetScript *_zs){
    return 10.5;
}

int main(){
	ZetScript zs;

	// Registers function interface
    zs.registerFunction("returnFloat",returnFloat);

    // Evaluates a script that calls the registered function and prints its result by console
    zs.eval(
        "Console::outln(\"result : \"+returnFloat());"
 	);
    return 0;
}

