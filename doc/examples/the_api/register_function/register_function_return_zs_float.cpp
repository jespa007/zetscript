#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::zs_float;

// Definition of the native function interface returnFloat
zs_float returnFloat(ZetScript *_zs){
    return 10.5;
}

int main(){
	ZetScript zs;

	// bind native function returnFloat named as 'returnFloat'
    zs.registerFunction("returnFloat",returnFloat);

    // Eval script that calls native function 'returnFloat'
    zs.eval(
        "Console::outln(\"result : \"+returnFloat());"
 	);

    return 0;
}

