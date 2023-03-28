#include "zetscript.h"

using zetscript::ZetScript;

// Definition of the native function interface returnBoolean
bool returnBoolean(ZetScript *_zs){
    return true;
}

int main(){
	ZetScript zs;

	// bind native function returnBoolean named as 'returnBoolean'
    zs.bindFunction("returnBoolean",returnBoolean);

    // Eval script that calls native function 'returnBoolean'
    zs.eval(
        "Console::outln(\"result : \"+returnBoolean());"
 	);

    return 0;
}

