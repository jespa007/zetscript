#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::StringScriptObject;

// Definition of the native function interface returnString
StringScriptObject *returnString(ZetScript *_zs){
	StringScriptObject *string=_zs->newStringScriptObject();
    string->set("Hello world (StringScriptObject)");
    return string;
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
