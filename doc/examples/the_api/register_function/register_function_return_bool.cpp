#include "zetscript.h"

using zetscript::ZetScript;

// C function
bool returnBoolean(ZetScript *_zs){
    return true;
}

int main(){

	ZetScript zs;

	// register C function
    zs.registerFunction("returnBoolean",returnBoolean);

    // Eval script that calls C function and prints its result by console
    zs.eval(
        "Console::outln(\"result : \"+returnBoolean());"
 	);

    return 0;
}

