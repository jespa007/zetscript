#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::zs_int;

// Definition of the native function interface returnInteger
zs_int returnInteger(ZetScript *_zs){
    return 10;
}

int main(){
	ZetScript zs;

	// bind native function returnInteger named as 'returnInteger'
    zs.bindFunction("returnInteger",returnInteger);

    // Eval script that calls native function 'returnInteger'
    zs.eval(
        "Console::outln(\"result : \"+returnInteger());"
 	);

    return 0;
}

