#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::zs_string;

// Definition of the native function interface returnString
zs_string returnString(ZetScript *_zs){
    return "Hello world (zs_string)";
}

int main(){
	ZetScript zs;

	// bind native function returnString named as 'returnString'
    zs.bindFunction("returnString",returnString);

    // Eval script that calls native function 'returnString'
    zs.eval(
        "Console::outln(\"result : \"+returnString());"
 	);

    return 0;
}

