#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::StringScriptObject;

// c function
void paramString(ScriptEngine *_zs, StringScriptObject *_string){
    printf("String : '%s' (StringScriptObject *)\n",_string->get().toConstChar());
}

int main(){
	ScriptEngine zs;

    zs.registerFunction("paramString",paramString);

    // call c function with string param
    zs.compileAndRun(
        "paramString(\"Hello World\");"
 	);


 	return 0;
}

