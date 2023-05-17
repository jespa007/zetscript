#include "zetscript.h"

using zetscript::ZetScript;

// ZetScript C++ interface function
bool returnBoolean(ZetScript *_zs){
    return true;
}

int main(){

	ZetScript zs;

	// Registers function interface
    zs.registerFunction("returnBoolean",returnBoolean);

    // Evaluates a script that calls the registered function and prints its result by console
    zs.eval(
        "Console::outln(\"result : \"+returnBoolean());"
 	);

    return 0;
}

