#include "zetscript.h"

using zetscript::ScriptEngine;
using zetscript::StringScriptObject;

// c function
void paramString(ScriptEngine *_se, StringScriptObject *_string){
    printf("String : '%s' (StringScriptObject *)\n",_string->get().toConstChar());
}

int main(){
	ScriptEngine se;

    se.registerFunction("paramString",paramString);

    // call c function with string param
    se.compileAndRun(
        "paramString(\"Hello World\");"
 	);


 	return 0;
}

