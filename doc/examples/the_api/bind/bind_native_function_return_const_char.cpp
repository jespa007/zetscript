#include "zetscript.h"

using zetscript::ZetScript;

// Definition of the native function interface returnString
const char * returnString(ZetScript *_zs){
    return "Hello world (const char *)";
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

